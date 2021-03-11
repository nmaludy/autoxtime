#include <autoxtime/db/DbListener.h>
#include <autoxtime/config/ConfigStore.h>

#include <iostream>

#include <QDebug>

AUTOXTIME_NAMESPACE_BEG

DbListener::DbListener(const QString& channel,
                       QObject* pParent)
    : QObject(pParent),
      mChannel(channel),
      mDatabase(QSqlDatabase::addDatabase("QPSQL", "autoxtime::DbListener::" + channel))
{
  // TODO try to reconnect in a timer loop
  mDatabase.setDatabaseName(ConfigStore::valueCast("db/database", QString("autoxtime")));
  mDatabase.setUserName(ConfigStore::valueCast("db/username", QString()));
  mDatabase.setPassword(ConfigStore::valueCast("db/password", QString()));
  mDatabase.setHostName(ConfigStore::valueCast("db/host", QString("localhost")));
  mDatabase.setPort(ConfigStore::valueCast<int>("db/port", 5432));
  if (!mDatabase.open()) {
    qFatal(QString("Failed to open database:" + mDatabase.lastError().text()).toLocal8Bit());
  }

  // hook up our listener before subscribing
  connect(mDatabase.driver(),
          qOverload<const QString&, QSqlDriver::NotificationSource , const QVariant&>(&QSqlDriver::notification),
          this,
          &DbListener::notification);
  // subscribe
  mDatabase.driver()->subscribeToNotification(channel);
}


void DbListener::notification(const QString& name,
                              QSqlDriver::NotificationSource source,
                              const QVariant& payload)
{
  qInfo().nospace() << "Notification name=" << name 
                    << " payload=" << payload.toString();
}

AUTOXTIME_NAMESPACE_END
