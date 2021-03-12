#include <autoxtime/db/DriverModel.h>

#include <autoxtime/db/DbConnection.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include <QtDebug>

AUTOXTIME_DB_NAMESPACE_BEG

const std::string DriverModel::TABLE = "driver";
const QString DriverModel::TABLE_Q = QString::fromStdString(DriverModel::TABLE);

DriverModel::DriverModel(std::shared_ptr<DbConnection> pConnection)
    : mpConnection(pConnection ? pConnection : std::make_shared<DbConnection>())
{}

std::vector<std::shared_ptr<autoxtime::proto::Driver> > DriverModel::list()
{
  // TODO should we cache this somewhere, maybe static since it never changes?
  const google::protobuf::Descriptor* p_desc = autoxtime::proto::Driver::GetDescriptor();
  int field_count = p_desc->field_count();
  std::unordered_map<std::string, const google::protobuf::FieldDescriptor*> names_to_fds;
  QStringList fields;
  for (int i = 0; i < field_count; ++i)
  {
    const google::protobuf::FieldDescriptor* p_fd = p_desc->field(i);
    fields << QString::fromStdString(p_fd->name());
    names_to_fds[p_fd->name()] = p_fd;
  }

  // if we initialize the query with the query string, it will execute immediate and
  // we can't capture the error message
  QSqlQuery query(mpConnection->database());
  query.prepare("SELECT " + fields.join(',') + " FROM " + DriverModel::TABLE_Q);

  std::vector<std::shared_ptr<autoxtime::proto::Driver> > results;
  if (query.exec())
  {
    results.reserve(query.size());
    
    const QSqlRecord record = query.record();
    const int column_count = record.count();
    std::vector<std::string> col_names;
    col_names.reserve(column_count);
    for (int c = 0; c < column_count; ++c)
    {
      col_names.push_back(record.fieldName(c).toStdString());
    }

    while (query.next())
    {
      std::shared_ptr<autoxtime::proto::Driver> p_driver =
          std::make_shared<autoxtime::proto::Driver>();
      const google::protobuf::Reflection* p_refl = p_driver->GetReflection();
      for (int c = 0; c < column_count; ++c)
      {
        const std::string& col_name = col_names.at(c);
        const google::protobuf::FieldDescriptor* p_fd = names_to_fds[col_name];
        setVariantToField(p_driver.get(), p_refl, p_fd, query.value(c));
      }
      results.push_back(p_driver);
    }
  }
  else
  {
    // TODO what in case of an error?
    qCritical().nospace() << "Error executing query '" << query.lastQuery() << "' - "
                          << query.lastError().text();
  }
  return results;
}

bool DriverModel::setVariantToField(google::protobuf::Message* pMessage,
                                    const google::protobuf::Reflection* pReflection,
                                    const google::protobuf::FieldDescriptor* pField,
                                    const QVariant& var)
{
  switch (pField->cpp_type())
  {
    // for the int types, use the width based type names because its less confusing
    case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
      pReflection->SetInt32(pMessage, pField, var.value<std::int32_t>());
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
      pReflection->SetInt64(pMessage, pField, var.value<std::int64_t>());
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
      pReflection->SetUInt32(pMessage, pField, var.value<std::uint32_t>());
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
      pReflection->SetUInt64(pMessage, pField, var.value<std::uint64_t>());
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
      pReflection->SetDouble(pMessage, pField, var.toFloat());
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
      pReflection->SetFloat(pMessage, pField, var.toFloat());
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
      pReflection->SetBool(pMessage, pField, var.toBool());
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
      pReflection->SetEnumValue(pMessage, pField, var.toInt());
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
      pReflection->SetString(pMessage, pField, var.toString().toStdString());
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
      qCritical().nospace() << "Error setting field '"
                            << QString::fromStdString(pField->full_name())
                            << "' "
                            << "message types are not yet supported for parsing";
      return false;
      break;
  }
  return true;
}

// bool create(const autoxtime::proto::Driver& driver);
// bool update(const autoxtime::proto::Driver& driver);
// bool findById(int id);
// bool findBy(const autoxtime::proto::Driver& driver);
bool DriverModel::findBy(const autoxtime::proto::Driver& driver)
{
  DbConnection conn;
  QSqlDatabase db = conn.database();
  if (driver.has_first_name())
  {
    
  }
  return false;
}

AUTOXTIME_DB_NAMESPACE_END
