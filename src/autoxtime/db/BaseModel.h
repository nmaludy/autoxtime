#ifndef AUTOXTIME_DB_BASEMODEL
#define AUTOXTIME_DB_BASEMODEL

#include <autoxtime/db/db.h>
#include <memory>
#include <unordered_map>
#include <QObject>
#include <QString>
#include <QtSql>

namespace google::protobuf {
class Descriptor;
class FieldDescriptor;
class Message;
class Reflection;
} // end namespace google::protobuf

AUTOXTIME_DB_NAMESPACE_BEG

class DbConnection;

class BaseModel : public QObject
{
  Q_OBJECT;

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

  template <typename T>
  inline std::vector<std::shared_ptr<T> > listT();

  //////////////////// create
  virtual int createMessage(const google::protobuf::Message* pMessage);

  template <typename T>
  inline int createT(const T* pMessage);

  template <typename T>
  inline int createT(const T& pMessage);

  //////////////////// update
  virtual int updateMessage(const google::protobuf::Message* pMessage);

  template <typename T>
  inline int updateT(const T* pMessage);

  template <typename T>
  inline int updateT(const T& pMessage);

  //////////////////// destroy
  virtual int destroyById(int id);

  //////////////////// find
  // TODO - should combined list() and find() ?
  virtual std::vector<std::shared_ptr<google::protobuf::Message> > findMessage(const google::protobuf::Message& prototype);

  template <typename T>
  inline std::vector<std::shared_ptr<T> > findT(const T& prototype);

  virtual std::vector<std::shared_ptr<google::protobuf::Message> > findMessageById(int id);

  template <typename T>
  inline std::vector<std::shared_ptr<T> > findByIdT(int id);


  //////////////////// behind the scenes stuff
  QVariant getFieldVariant(const google::protobuf::Message& message,
                           const google::protobuf::FieldDescriptor* pField);
  bool setFieldVariant(google::protobuf::Message* pMessage,
                       const google::protobuf::FieldDescriptor* pField,
                       const QVariant& var);
  QString wherePrototype(const google::protobuf::Message& message);
  std::vector<std::shared_ptr<google::protobuf::Message> > parseQueryResults(QSqlQuery& query);

  template <typename T>
  inline std::vector<std::shared_ptr<T> > messagesToT(std::vector<std::shared_ptr<google::protobuf::Message> >& messages);

  inline std::shared_ptr<DbConnection>& connection();

 private:
  int createOrUpdateMessage(const google::protobuf::Message* pMessage, bool bCreate);

  const std::string mTable;
  const QString mTableQ;
  const std::string mPrimaryKey;
  const QString mPrimaryKeyQ;
  const google::protobuf::Descriptor* mpDescriptor;
  const google::protobuf::Reflection* mpReflection;
  const google::protobuf::Message* mpPrototype;
  std::shared_ptr<DbConnection> mpConnection;
  QStringList mFieldNames;
  std::vector<const google::protobuf::FieldDescriptor*> mFieldDescriptors;
  std::unordered_map<std::string, const google::protobuf::FieldDescriptor*> mFieldNamesToFds;
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

template <typename T>
inline std::vector<std::shared_ptr<T> > BaseModel
::messagesToT(std::vector<std::shared_ptr<google::protobuf::Message> >& messages)
{
  std::vector<std::shared_ptr<T> > results;
  results.reserve(messages.size());
  for (std::shared_ptr<google::protobuf::Message>& msg : messages)
  {
    results.push_back(std::dynamic_pointer_cast<T>(msg));
  }
  return results;
}

template <typename T>
inline std::vector<std::shared_ptr<T> > BaseModel::listT()
{
  std::vector<std::shared_ptr<google::protobuf::Message> > msgs = listMessage();
  return messagesToT<T>(msgs);
}

template <typename T>
inline std::vector<std::shared_ptr<T> > BaseModel::findT(const T& prototype)
{
  std::vector<std::shared_ptr<google::protobuf::Message> > msgs = findMessage(prototype);
  return messagesToT<T>(msgs);
}

template <typename T>
inline std::vector<std::shared_ptr<T> > BaseModel::findByIdT(int id)
{
  std::vector<std::shared_ptr<google::protobuf::Message> > msgs = findMessageById(id);
  return messagesToT<T>(msgs);
}

template <typename T>
inline int BaseModel::createT(const T* pMessage)
{
  return createMessage(dynamic_cast<const google::protobuf::Message*>(pMessage));
}

template <typename T>
inline int BaseModel::createT(const T& message)
{
  return createT(&message);
}

template <typename T>
inline int BaseModel::updateT(const T* pMessage)
{
  return updateMessage(dynamic_cast<const google::protobuf::Message*>(pMessage));
}

template <typename T>
inline int BaseModel::updateT(const T& message)
{
  return updateT(&message);
}

inline std::shared_ptr<DbConnection>& BaseModel::connection()
{
  return mpConnection;
}

AUTOXTIME_DB_NAMESPACE_END

#endif // AUTOXTIME_DB_DRIVER
