#ifndef AUTOXTIME_DB_BASEMODEL
#define AUTOXTIME_DB_BASEMODEL

#include <autoxtime/db/db.h>
#include <memory>
#include <unordered_map>
#include <QObject>
#include <QString>
#include <QtSql>

namespace google { namespace protobuf { class Descriptor; } }
namespace google { namespace protobuf { class FieldDescriptor; } }
namespace google { namespace protobuf { class Message; } }
namespace google { namespace protobuf { class Reflection; } }

AUTOXTIME_DB_NAMESPACE_BEG

class DbConnection;

class BaseModel : public QObject
{
  Q_OBJECT

 public:
  BaseModel(const std::string& table,
            const std::string& primaryKey,
            const google::protobuf::Descriptor* pDescriptor,
            const google::protobuf::Reflection* pReflection,
            std::shared_ptr<DbConnection> pConnection = std::shared_ptr<DbConnection>(),
            QObject* pParent = nullptr);
  virtual ~BaseModel() = default;

  // pure virtual, required to be implemented by sub-classes
  inline const std::string& table() const;
  inline const QString&     tableQ() const;
  inline const std::string& primaryKey() const;
  inline const QString&     primaryKeyQ() const;

  //////////////////// list
  virtual std::vector<std::shared_ptr<google::protobuf::Message> > listMessage();

  //////////////////// create
  // returns a list of messages with the primary key field set to the newly created ID
  virtual std::vector<std::shared_ptr<google::protobuf::Message> > createMessage(const google::protobuf::Message* pMessage);

  //////////////////// update
  virtual std::vector<std::shared_ptr<google::protobuf::Message> > updateMessage(const google::protobuf::Message* pMessage);

  //////////////////// destroy
  virtual std::int64_t destroyById(std::int64_t id);

  //////////////////// find
  // TODO - should combined list() and find() ?
  virtual std::vector<std::shared_ptr<google::protobuf::Message> > findMessage(const google::protobuf::Message& prototype);

  virtual std::vector<std::shared_ptr<google::protobuf::Message> > findMessageById(std::int64_t id);


  virtual std::vector<std::shared_ptr<google::protobuf::Message> > findMessageCustom(const QString& custom,
                                                                                    const std::unordered_map<QString, QVariant>& bindings = std::unordered_map<QString, QVariant>());

  //////////////////// subscriptions and notifications
  void subscribeToNotifications();

  //////////////////// behind the scenes stuff
  static QVariant getFieldVariant(const google::protobuf::Message& message,
                                  const google::protobuf::FieldDescriptor* pField);
  static bool setFieldVariant(google::protobuf::Message* pMessage,
                              const google::protobuf::FieldDescriptor* pField,
                              const QVariant& var);
  QString wherePrototype(const google::protobuf::Message& message);
  std::vector<std::shared_ptr<google::protobuf::Message> > parseQueryResults(QSqlQuery& query);

  inline std::shared_ptr<DbConnection>& connection();

 signals:
  void notification(const std::shared_ptr<google::protobuf::Message>& pMessage,
                    const QDateTime& timestamp,
                    const QString& operation);

 public slots:
  // DbEmitter::notification -> this
  void emitNotification(const std::shared_ptr<google::protobuf::Message>& pMessage,
                        const QDateTime& timestamp,
                        const QString& operation);

 protected:
  std::vector<std::shared_ptr<google::protobuf::Message> > createOrUpdateMessage(const google::protobuf::Message* pMessage, bool bCreate);
  static QStringList fieldNames(const google::protobuf::Descriptor* pDescriptor);
  static std::vector<const google::protobuf::FieldDescriptor*> fieldDescriptors(const google::protobuf::Descriptor* pDescriptor);
  static std::unordered_map<std::string, const google::protobuf::FieldDescriptor*> fieldNamesToFds(const google::protobuf::Descriptor* pDescriptor);

  const std::string mTable;
  const QString mTableQ;
  const std::string mPrimaryKey;
  const QString mPrimaryKeyQ;
  const google::protobuf::Descriptor* mpDescriptor;
  const google::protobuf::Reflection* mpReflection;
  const google::protobuf::Message* mpPrototype;
  std::shared_ptr<DbConnection> mpConnection;
  const QStringList mFieldNames;
  const std::vector<const google::protobuf::FieldDescriptor*> mFieldDescriptors;
  const std::unordered_map<std::string, const google::protobuf::FieldDescriptor*> mFieldNamesToFds;
};


inline const std::string& BaseModel::table() const
{
  return mTable;
}

inline const QString& BaseModel::tableQ() const
{
  return mTableQ;
}

inline const std::string& BaseModel::primaryKey() const
{
  return mPrimaryKey;
}

inline const QString& BaseModel::primaryKeyQ() const
{
  return mPrimaryKeyQ;
}

inline std::shared_ptr<DbConnection>& BaseModel::connection()
{
  return mpConnection;
}

AUTOXTIME_DB_NAMESPACE_END

#endif // AUTOXTIME_DB_DRIVER
