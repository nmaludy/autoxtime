#ifndef AUTOXTIME_DB_DBLISTENER
#define AUTOXTIME_DB_DBLISTENER

#include <autoxtime/db/db.h>
#include <QtSql/QtSql>
#include <QThread>
#include <memory>

class QSemaphore;
namespace autoxtime { namespace db { class DbConnection; } }

AUTOXTIME_DB_NAMESPACE_BEG

class DbEmitter : public QObject
{
  Q_OBJECT
 protected:
  DbEmitter(const google::protobuf::Message& prototype);

 signals:
  void notification(const QString& name,
                    QSqlDriver::NotificationSource source,
                    std::shared_ptr<google::protobuf::Message>& pMessage);

 public slots:
  // DbListener::notification -> this
  void recvNotification(const QString& name,
                        QSqlDriver::NotificationSource source,
                        const QString& payload);

 private:
  friend class DbListener;
  std::unique<google::protobuf::Message> mpPrototype;
};

////////////////////////////////////////////////////////////////////////////////

class DbListener : public QThread
{
  Q_OBJECT
 public:
  static DbListener& instance();

  virtual ~DbListener() = default;

  DbEmitter* emitter(const QString& channel,
                     const google::protobuf::Message& prototype);
  void subscribe(const QString& channel);

 protected slots:
  // subscribe() -> this
  void subscribeSlot(const QString& channel);
  // QSqlDriver -> this
  void recvNotification(const QString& name,
                        QSqlDriver::NotificationSource source,
                        const QVariant& payload);

 signals:
  // subscribe() -> subscribeSlot()
  void subscribeSignal(const QString& channel);
  // QSqlDriver -> this
  void notification(const QString& name,
                    QSqlDriver::NotificationSource source,
                    const QVariant& payload);

 protected:
  virtual void run() override;

 private:
  DbListener();
  friend class DbEmitter;

  std::mutex mEmitterMutex;
  std::unordered_map<QString, std::unique_ptr<DbEmitter>> mEmitters;

  std::unique_ptr<QSemaphore> mpStartSemaphore;
  std::unique_ptr<DbConnection> mpConnection;
};

AUTOXTIME_DB_NAMESPACE_END

#endif // AUTOXTIME_DB_DBLISTENER
