#ifndef AUTOXTIME_DB_DBCONNECTION
#define AUTOXTIME_DB_DBCONNECTION

#include <autoxtime/db/db.h>
#include <QtSql/QtSql>

class QTimer;

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
  explicit DbConnection(QObject* pParent = nullptr);
  explicit DbConnection(const QString& name, QObject* pParent = nullptr);
  virtual ~DbConnection();

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
  QTimer* mpTimer;
};

AUTOXTIME_DB_NAMESPACE_END

#endif // AUTOXTIME_DB_DBCONNECTION
