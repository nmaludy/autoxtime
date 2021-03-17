#ifndef AUTOXTIME_DB_DBCONNECTION
#define AUTOXTIME_DB_DBCONNECTION

#include <autoxtime/db/db.h>
#include <QtSql/QtSql>
#include <QTimer>

AUTOXTIME_DB_NAMESPACE_BEG

enum DbConnectionState
{
  INITIALIZE,
  CONNECTED,
  DISCONNECTED
};

class DbConnection : public QObject
{
  Q_OBJECT

 public:

  DbConnection(QObject* pParent = nullptr);
  DbConnection(const QString& name, QObject* pParent = nullptr);

  QSqlDatabase& database()
  {
    return mDatabase;
  }

  const QString& name() const
  {
    return mName;
  }

 signals:
  void disconnected(DbConnection* pConnection);
  void connected(DbConnection* pConnection);

 public slots:
  void tryConnect();

 private:
  QString mName;
  DbConnectionState mState;
  QSqlDatabase mDatabase;
  QTimer mTimer;
};

AUTOXTIME_DB_NAMESPACE_END

#endif // AUTOXTIME_DB_DBCONNECTION
