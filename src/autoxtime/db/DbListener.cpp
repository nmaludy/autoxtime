#include <autoxtime/db/DbListener.h>
#include <autoxtime/db/DbConnection.h>
#include <autoxtime/log/Log.h>

#include <QSemaphore>

#include <iostream>

AUTOXTIME_DB_NAMESPACE_BEG

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

DbListenerThread::DbListenerThread()
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

DbListenerThread& DbListenerThread::instance()
{
  static DbListenerThread thread;
  return thread;
}

void DbListenerThread::run()
{
  AXT_DEBUG << "In run thread"
            << QString("autoxtime::thread_") + QString::number((intptr_t)QThread::currentThreadId());
  mpConnection = std::make_unique<DbConnection>();

  // hook up our listener before subscribing
  connect(mpConnection->database().driver(),
          qOverload<const QString&, QSqlDriver::NotificationSource , const QVariant&>(&QSqlDriver::notification),
          this, &DbListenerThread::notification);
  connect(this, &DbListenerThread::subscribeSignal,
          this, &DbListenerThread::subscribeSlot);

  // avoid race condition on start, wait until we're connected
  mpStartSemaphore->release(1);
  QThread::run();
}

void DbListenerThread::subscribe(const QString& channel)
{
  AXT_DEBUG << "subscribe called from thread: " << QString("autoxtime::thread_") + QString::number((intptr_t)QThread::currentThreadId());

  // send signal to our background thread that we want to subscribe to this new channel
  emit subscribeSignal(channel);
}

void DbListenerThread::subscribeSlot(const QString& channel)
{
  AXT_DEBUG << "subscribeSlot called in thread: " << QString("autoxtime::thread_") + QString::number((intptr_t)QThread::currentThreadId());
  mpConnection->database().driver()->subscribeToNotification(channel);
}

void DbListenerThread::notification(const QString& name,
                              QSqlDriver::NotificationSource source,
                              const QVariant& payload)
{
  // commented out because they are unused and causing build failures
  // QJsonDocument payload_doc = QJsonDocument::fromJson(payload.toString().toUtf8());
  // QJsonObject payload_json = payload_doc.object();
  // QJsonObject payload_data = payload_json.value("data").toObject();

  AXT_DEBUG << "Notification name=" << name
            << " payload=" << payload.toString()
            << "\n"
            << "  in thread: " << QString("autoxtime::thread_") + QString::number((intptr_t)QThread::currentThreadId());;
}

AUTOXTIME_DB_NAMESPACE_END
