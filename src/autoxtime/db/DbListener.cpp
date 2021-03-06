#include <autoxtime/db/DbListener.h>
#include <autoxtime/db/DbConnection.h>
#include <autoxtime/log/Log.h>

#include <google/protobuf/util/json_util.h>

#include <QSemaphore>

#include <iostream>

AUTOXTIME_DB_NAMESPACE_BEG

DbEmitter::DbEmitter(const google::protobuf::Message& prototype)
    : QObject(),
      mpPrototype(prototype.New())
{}

DbEmitter::~DbEmitter()
{}

void DbEmitter::emitNotification(const QString& name,
                                 QSqlDriver::NotificationSource source,
                                 const QVariant& payload)
{
  // Note: as a measure of efficiency we convert and parse the JSON data once
  //       then pass the parsed google::protobuf::Message to the emitted signal
  //       so that we dont' have to parse the message over/over in each listener
  QByteArray payload_ba = payload.toString().toUtf8();
  QJsonDocument payload_doc = QJsonDocument::fromJson(payload_ba);
  QJsonObject payload_json = payload_doc.object();
  // payload contains following fields:
  // "timestamp" - string
  // "operation" - string (UPDATE, INSERT, DELETE, etc)
  // "data" - data/row that was changed, encoded as JSON
  QString timestamp_str = payload_json.value("timestamp").toString();
  QString operation = payload_json.value("operation").toString();
  QJsonDocument data_doc(payload_json.value("data").toObject());

  // parse ISO 8601 timestamp
  QDateTime timestamp = QDateTime::fromString(timestamp_str, Qt::ISODate);

  // convert Qt string to string we can use for parsing
  QString data_json(data_doc.toJson());
  std::string data_str(data_json.toStdString());
  google::protobuf::StringPiece piece(data_str.data());

  // create a new message that will be filled in with the data from the JSON string
  std::shared_ptr<google::protobuf::Message> msg(mpPrototype->New());
  // parse JSON into the message
  google::protobuf::util::Status status =
      google::protobuf::util::JsonStringToMessage(piece, msg.get());
  if (status.ok())
  {
    AXT_DEBUG << "DbEmitter - Notification name=" << name
              << " thread=" << QString("autoxtime::thread_") + QString::number((intptr_t)QThread::currentThreadId())
              << " timestamp=" << timestamp
              << " operation=" << operation
              << " payload=" << payload_ba << "\n"
              << " msg=" << msg->DebugString();
    emit notification(msg, timestamp, operation);
  }
  else
  {
    AXT_ERROR << "Error parsing JSON payload error='"<< status.ToString() << "'"
              << " channel=" << name
              << " timestamp=" << timestamp
              << " operation=" << operation
              << " payload=" << payload;
  }
}

////////////////////////////////////////////////////////////////////////////////

DbListener::DbListener()
    : QThread(),
      mpStartSemaphore(new QSemaphore()),
      mpConnection()
{
  qRegisterMetaType<std::shared_ptr<google::protobuf::Message>>();
  start();

  // avoid race condition on start, wait until we're connected
  mpStartSemaphore->acquire(1);

  // move this object to "this" thread so that signals cross the right boundaries
  moveToThread(this);
}

DbListener& DbListener::instance()
{
  static DbListener thread;
  return thread;
}

void DbListener::run()
{
  AXT_DEBUG << "In DbListener thread"
            << QString("autoxtime::thread_") + QString::number((intptr_t)QThread::currentThreadId());
  {
    std::unique_lock<std::mutex> lock(mConnectionMutex);
    mpConnection = std::make_unique<DbConnection>(this);
  }

  // hook up our listener before subscribing
  connect(mpConnection->database().driver(),
          qOverload<const QString&, QSqlDriver::NotificationSource , const QVariant&>(&QSqlDriver::notification),
          this, &DbListener::recvNotification);
  connect(this, &DbListener::subscribeSignal,
          this, &DbListener::subscribeSlot);

  // avoid race condition on start, wait until we're connected
  mpStartSemaphore->release(1);

  // run our event loop
  QThread::run();

  {
    AXT_DEBUG << "Stopping DbListener thread"
              << QString("autoxtime::thread_") + QString::number((intptr_t)QThread::currentThreadId());
    // destroy the connection, still in our thread, to avoid warnings about tearing
    // down the QTimers in another thread
    std::unique_lock<std::mutex> lock(mConnectionMutex);
    mpConnection.reset();
  }
}

DbEmitter* DbListener::emitter(const google::protobuf::Message& prototype,
                               const QString& tableName)
{
  std::unique_lock<std::mutex> lock(mEmitterMutex);
  std::unordered_map<QString, std::unique_ptr<DbEmitter>>::iterator iter =
      mEmitters.find(tableName);
  if (iter != mEmitters.end())
  {
    return iter->second.get();
  }
  else
  {
    mEmitters[tableName] = std::make_unique<DbEmitter>(prototype);
    return mEmitters[tableName].get();
  }
}

void DbListener::subscribe(const google::protobuf::Message& prototype,
                                 const QString& tableName)
{
  AXT_DEBUG << "subscribe for channel "
            << tableName
            << " called from thread: " << QString("autoxtime::thread_") + QString::number((intptr_t)QThread::currentThreadId());

  // create an emitter for this subscription, just in case one doesn't exist
  emitter(prototype, tableName);

  // send signal to our background thread that we want to subscribe to this new channel
  emit subscribeSignal(tableName);
}

void DbListener::subscribeSlot(const QString& channel)
{
  AXT_DEBUG << "subscribeSlot for channel "
            << channel
            << " called in thread: " << QString("autoxtime::thread_") + QString::number((intptr_t)QThread::currentThreadId());
  std::unique_lock<std::mutex> lock(mConnectionMutex);
  if (mpConnection)
  {
    mpConnection->database().driver()->subscribeToNotification(channel);
  }
}

void DbListener::recvNotification(const QString& name,
                                  QSqlDriver::NotificationSource source,
                                  const QVariant& payload)
{
  std::unique_lock<std::mutex> lock(mEmitterMutex);
  std::unordered_map<QString, std::unique_ptr<DbEmitter>>::iterator iter = mEmitters.find(name);
  if (iter != mEmitters.end())
  {
    AXT_DEBUG << "Notification found_emitter=true name=" << name
              << " thread= " << QString("autoxtime::thread_") + QString::number((intptr_t)QThread::currentThreadId())
              << " payload=" << payload.toString();
    iter->second->emitNotification(name, source, payload);
  }
  else
  {
    AXT_DEBUG << "Notification found_emitter=false name=" << name
              << " thread= " << QString("autoxtime::thread_") + QString::number((intptr_t)QThread::currentThreadId())
              << " payload=" << payload.toString();
  }
  emit notification(name, source, payload);
}

AUTOXTIME_DB_NAMESPACE_END
