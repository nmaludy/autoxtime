#include <autoxtime/db/DbConnection.h>
#include <autoxtime/config/ConfigStore.h>

#include <iostream>

#include <QDebug>
#include <QTimer>

AUTOXTIME_DB_NAMESPACE_BEG

DbConnection::DbConnection(QObject* pParent)
    : DbConnection(QString(), pParent)
{}

DbConnection::DbConnection(const QString& name,
                           QObject* pParent)
    : QObject(pParent),
      mState(DbConnectionState::INITIALIZE),
      mName(!name.isEmpty() ? name : QString("autoxtime::thread_") + QString::number((intptr_t)QThread::currentThreadId())),
      mDatabase(),
      mTimer()
{
  qInfo().nospace() << "DbConnection() - Creating db connection" << mName;

  // TODO  - how should we cleanup  the database connection?
  // problems:
  //  - currently connections are per-thread, so multiple models share the same connection
  //  - should we make each model per-thread a unique connection name so we can create / destroy
  //    connections properly?
  //  - shuold we create a thread-safe connection pool?
  //  - should we just have one connection per thread and don't worry about it
  if (QSqlDatabase::contains(mName))
  {
    mDatabase = QSqlDatabase::database(mName);
  }
  else
  {
    mDatabase = QSqlDatabase::addDatabase("QPSQL", mName);
    mDatabase.setDatabaseName(ConfigStore::valueCast("db/database", QString("autoxtime")));
    mDatabase.setUserName(ConfigStore::valueCast("db/username", QString()));
    mDatabase.setPassword(ConfigStore::valueCast("db/password", QString()));
    mDatabase.setHostName(ConfigStore::valueCast("db/host", QString("localhost")));
    mDatabase.setPort(ConfigStore::valueCast<int>("db/port", 5432));
  }

  connect(&mTimer, &QTimer::timeout,
          this,    &DbConnection::tryConnect);

  tryConnect();
  mTimer.start(ConfigStore::valueCast("db/reconnect_interval_ms", 5000));
}

void DbConnection::tryConnect()
{
  if (!mDatabase.isOpen())
  {
    if (mDatabase.open())
    {
      mState = DbConnectionState::CONNECTED;
      emit connected(this);
    }
    else
    {
      // don't want to keep sending signals that we're disconnected, only on change
      if (mState != DbConnectionState::DISCONNECTED)
      {
        mState = DbConnectionState::DISCONNECTED;
        emit disconnected(this);
      }
      qWarning().nospace() << "Failed to open database: " + mDatabase.lastError().text();
    }
  }
}

AUTOXTIME_DB_NAMESPACE_END
