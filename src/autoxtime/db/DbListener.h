#ifndef AUTOXTIME_DB_DBLISTENER
#define AUTOXTIME_DB_DBLISTENER

#include <autoxtime/autoxtime.h>
#include <QtSql/QtSql>
#include <memory>
#include <autoxtime/db/DbConnection.h>

AUTOXTIME_NAMESPACE_BEG

class DbListener : public QObject
{
  Q_OBJECT

 public:
  // subscribe to events on this channel:
  // https://www.postgresql.org/docs/13/sql-notify.html
  DbListener(const QString& channel = QString(),
             QObject* pParent = nullptr);

 public slots:
  void notification(const QString& name,
                    QSqlDriver::NotificationSource source,
                    const QVariant& payload);
  
 private:
  QString mChannel;
  std::unique_ptr<DbConnection> mpConnection;
};

AUTOXTIME_NAMESPACE_END

#endif // AUTOXTIME_DB_DBLISTENER
