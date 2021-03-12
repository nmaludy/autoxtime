#ifndef AUTOXTIME_DB_DRIVERMODEL
#define AUTOXTIME_DB_DRIVERMODEL

#include <autoxtime/db/db.h>
#include <autoxtime/proto/driver.pb.h>
#include <memory>
#include <QObject>
#include <QString>

namespace google::protobuf {
class Message;
class Reflection;
class FieldDescriptor;
} // end namespace google::protobuf

AUTOXTIME_DB_NAMESPACE_BEG

class DbConnection;

class DriverModel : public QObject
{
  Q_OBJECT;

 public:
  static const std::string TABLE;
  static const QString TABLE_Q;

  DriverModel(std::shared_ptr<DbConnection> pConnection = std::shared_ptr<DbConnection>());

  std::vector<std::shared_ptr<autoxtime::proto::Driver> > list();
  bool create(const autoxtime::proto::Driver& driver);
  bool update(const autoxtime::proto::Driver& driver);
  bool destroy(const autoxtime::proto::Driver& driver);

  bool findById(int id);
  bool findBy(const autoxtime::proto::Driver& driver);

  bool setVariantToField(google::protobuf::Message* pMessage,
                         const google::protobuf::Reflection* pReflection,
                         const google::protobuf::FieldDescriptor* pField,
                         const QVariant& var);

 private:
  std::shared_ptr<DbConnection> mpConnection;
};

AUTOXTIME_DB_NAMESPACE_END

#endif // AUTOXTIME_DB_DRIVER
