#ifndef AUTOXTIME_DB_DBLISTENER
#define AUTOXTIME_DB_DBLISTENER

#include <autoxtime/db/db.h>
#include <QtSql/QtSql>
#include <QThread>
#include <memory>

class QSemaphore;

AUTOXTIME_DB_NAMESPACE_BEG

class DbConnection;


class DbSubscriber : public QObject
{
  Q_OBJECT
 public:
  DbSubscriber(const QString& channel);

 signals:
  void subscribeSignal(const QString& channel);
};

class DbListenerThread : public QThread
{
  Q_OBJECT
 public:
  static DbListenerThread& instance();

  virtual ~DbListenerThread() = default;

  virtual void run() override;
  void subscribe(const QString& channel);

 public slots:
  void notification(const QString& name,
                    QSqlDriver::NotificationSource source,
                    const QVariant& payload);
  void subscribeSlot(const QString& channel);

 signals:
  void subscribeSignal(const QString& channel);

 private:
  DbListenerThread();


  std::unique_ptr<QSemaphore> mpStartSemaphore;
  std::unique_ptr<DbConnection> mpConnection;
};

AUTOXTIME_DB_NAMESPACE_END

#endif // AUTOXTIME_DB_DBLISTENER
