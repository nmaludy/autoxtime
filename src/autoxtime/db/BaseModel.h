#ifndef AUTOXTIME_DB_BASEMODEL
#define AUTOXTIME_DB_BASEMODEL

#include <autoxtime/db/db.h>
#include <memory>
#include <unordered_map>
#include <QObject>
#include <QString>

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
  BaseModel(const google::protobuf::Descriptor* pDescriptor,
            const google::protobuf::Reflection* pReflection,
            std::shared_ptr<DbConnection> pConnection = std::shared_ptr<DbConnection>());
  virtual ~BaseModel() = default;

  std::vector<std::shared_ptr<google::protobuf::Message> > listMessage();

  template <typename T>
  std::vector<std::shared_ptr<T> > listT();
  
  bool setFieldVariant(google::protobuf::Message* pMessage,
                       const google::protobuf::FieldDescriptor* pField,
                       const QVariant& var);

  // pure virtual, required to be implemented by sub-classes
  virtual const std::string& table() const = 0;
  virtual const QString&     tableQ() const = 0;

  inline std::shared_ptr<DbConnection>& connection();

 private:
  const google::protobuf::Descriptor* mpDescriptor;
  const google::protobuf::Reflection* mpReflection;
  const google::protobuf::Message* mpPrototype;
  std::shared_ptr<DbConnection> mpConnection;
  QStringList mFieldNames;
  std::unordered_map<std::string, const google::protobuf::FieldDescriptor*> mFieldNamesToFds;
};

template <typename T>
std::vector<std::shared_ptr<T> > BaseModel::listT()
{
  std::vector<std::shared_ptr<google::protobuf::Message> > msgs = listMessage();
  std::vector<std::shared_ptr<T> > results;
  results.reserve(msgs.size());
  for (std::shared_ptr<google::protobuf::Message>& msg : msgs)
  {
    results.push_back(std::dynamic_pointer_cast<T>(msg));
  }
  return results;
}

inline std::shared_ptr<DbConnection>& BaseModel::connection()
{
  return mpConnection;
}

AUTOXTIME_DB_NAMESPACE_END

#endif // AUTOXTIME_DB_DRIVER
