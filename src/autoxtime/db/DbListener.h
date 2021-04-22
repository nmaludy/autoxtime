#ifndef AUTOXTIME_DB_DBLISTENER
#define AUTOXTIME_DB_DBLISTENER

#include <autoxtime/db/db.h>
#include <QtSql/QtSql>
#include <QThread>
#include <memory>
#include <google/protobuf/message.h>

class QSemaphore;
namespace autoxtime { namespace db { class DbConnection; } }

AUTOXTIME_DB_NAMESPACE_BEG

class DbEmitter : public QObject
{
  Q_OBJECT
 public:
  DbEmitter(const google::protobuf::Message& prototype);
  virtual ~DbEmitter();

 signals:
  void notification(const std::shared_ptr<google::protobuf::Message>& pMessage,
                    const QDateTime& timestamp,
                    const QString& operation);

 public slots:
  // DbListener::notification -> this
  void emitNotification(const QString& name,
                        QSqlDriver::NotificationSource source,
                        const QVariant& payload);

 private:
  std::unique_ptr<google::protobuf::Message> mpPrototype;
};

////////////////////////////////////////////////////////////////////////////////

class DbListener : public QThread
{
  Q_OBJECT
 public:
  static DbListener& instance();

  virtual ~DbListener() = default;

  DbEmitter* emitter(const google::protobuf::Message& prototype,
                     const QString& tableName);
  void subscribe(const google::protobuf::Message& prototype,
                 const QString& tableName);

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

// needed so we can use signals/slots with this type
Q_DECLARE_METATYPE(std::shared_ptr<google::protobuf::Message>)

#endif // AUTOXTIME_DB_DBLISTENER
