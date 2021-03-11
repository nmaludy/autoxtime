#ifndef AUTOXTIME_DB_DBCONNECTION
#define AUTOXTIME_DB_DBCONNECTION

#include <autoxtime/autoxtime.h>
#include <QtSql/QtSql>
#include <QTimer>

AUTOXTIME_NAMESPACE_BEG

class DbConnection : public QObject
{
  Q_OBJECT

 public:
  DbConnection(const QString& name = QString(),
               QObject* pParent = nullptr);

  QSqlDatabase& database()
  {
    return mDatabase;
  }

  const QString& name() const
  {
    return mName;
  }

 public slots:
  void tryConnect();
  
 private:
  QString mName;
  QSqlDatabase mDatabase;
  QTimer mTimer;
};

AUTOXTIME_NAMESPACE_END

#endif // AUTOXTIME_DB_CONNECTION
