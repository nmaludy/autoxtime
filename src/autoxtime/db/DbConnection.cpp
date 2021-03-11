#include <autoxtime/db/DbConnection.h>
#include <autoxtime/config/ConfigStore.h>

#include <iostream>

#include <QDebug>
#include <QTimer>

AUTOXTIME_NAMESPACE_BEG

DbConnection::DbConnection(const QString& name,
                           QObject* pParent)
    : QObject(pParent),
      mName(!name.isEmpty() ? name : QString("autoxtime::thread_") + (intptr_t)QThread::currentThreadId()),
      mDatabase(QSqlDatabase::addDatabase("QPSQL", mName)),
      mTimer()
{
  mDatabase.setDatabaseName(ConfigStore::valueCast("db/database", QString("autoxtime")));
  mDatabase.setUserName(ConfigStore::valueCast("db/username", QString()));
  mDatabase.setPassword(ConfigStore::valueCast("db/password", QString()));
  mDatabase.setHostName(ConfigStore::valueCast("db/host", QString("localhost")));
  mDatabase.setPort(ConfigStore::valueCast<int>("db/port", 5432));

  connect(&mTimer, &QTimer::timeout,
          this,    &DbConnection::tryConnect);

  tryConnect();
  mTimer.start(ConfigStore::valueCast("db/reconnect_interval_ms", 5000));
}

void DbConnection::tryConnect()
{
  if (!mDatabase.isOpen())
  {
    if (!mDatabase.open())
    {
      qWarning().nospace() << "Failed to open database: " + mDatabase.lastError().text();
    }
  }
}

AUTOXTIME_NAMESPACE_END
