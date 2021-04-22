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
  std::string data_str(payload.toString().toStdString());
  google::protobuf::StringPiece piece(data_str.data());
  std::shared_ptr<google::protobuf::Message> msg(mpPrototype->New());
  google::protobuf::util::Status status =
      google::protobuf::util::JsonStringToMessage(piece, msg.get());
  if (status.ok())
  {
    
    AXT_DEBUG << "DbEmitter - Notification name=" << name
              << " payload=" << data_str
              << "\n"
              << "  in thread: " << QString("autoxtime::thread_") + QString::number((intptr_t)QThread::currentThreadId());;
    emit notification(name, source, msg);
  }
  else
  {
    AXT_ERROR << "Error parsing JSON payload: "<< status.ToString() << "\n"
              << "channel = " << name << "\n"
              << "payload = " << payload;
  }
}

////////////////////////////////////////////////////////////////////////////////

DbListener::DbListener()
    : QThread(),
      mpStartSemaphore(new QSemaphore()),
      mpConnection()
{
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
  AXT_DEBUG << "In run thread"
            << QString("autoxtime::thread_") + QString::number((intptr_t)QThread::currentThreadId());
  mpConnection = std::make_unique<DbConnection>();

  // hook up our listener before subscribing
  connect(mpConnection->database().driver(),
          qOverload<const QString&, QSqlDriver::NotificationSource , const QVariant&>(&QSqlDriver::notification),
          this, &DbListener::recvNotification);
  connect(this, &DbListener::subscribeSignal,
          this, &DbListener::subscribeSlot);

  // avoid race condition on start, wait until we're connected
  mpStartSemaphore->release(1);
  QThread::run();
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
    std::unique_ptr<DbEmitter> ptr = std::make_unique<DbEmitter>(prototype);
    // get reference to emitter ptr before we move it into the container
    DbEmitter* p_emitter = ptr.get();
    mEmitters[tableName] = std::move(ptr);
    return p_emitter;
  }
}

void DbListener::subscribe(const google::protobuf::Message& prototype,
                           const QString& tableName)
{
  AXT_DEBUG << "subscribe for channel "
            << tableName
            << " called from thread: " << QString("autoxtime::thread_") + QString::number((intptr_t)QThread::currentThreadId());

  // send signal to our background thread that we want to subscribe to this new channel
  emit subscribeSignal(tableName);
}

void DbListener::subscribeSlot(const QString& channel)
{
  AXT_DEBUG << "subscribeSlot for channel "
            << channel
            << " called in thread: " << QString("autoxtime::thread_") + QString::number((intptr_t)QThread::currentThreadId());
  mpConnection->database().driver()->subscribeToNotification(channel);
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
