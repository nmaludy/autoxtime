#include <autoxtime/db/BaseModel.h>

#include <autoxtime/db/DbConnection.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include <QtDebug>

AUTOXTIME_DB_NAMESPACE_BEG

BaseModel::BaseModel(const google::protobuf::Descriptor* pDescriptor,
                     const google::protobuf::Reflection* pReflection,
                     std::shared_ptr<DbConnection> pConnection)
    : QObject(),
      mpDescriptor(pDescriptor),
      mpReflection(pReflection),
      mpPrototype(pReflection->GetMessageFactory()->GetPrototype(mpDescriptor)),
      mpConnection(pConnection ? pConnection : std::make_shared<DbConnection>()),
      mFieldNames(),
      mFieldNamesToFds()
{
  // cache the mapping of field names to descriptors and the field name list
  int field_count = mpDescriptor->field_count();
  for (int i = 0; i < field_count; ++i)
  {
    const google::protobuf::FieldDescriptor* p_fd = mpDescriptor->field(i);
    mFieldNames << QString::fromStdString(p_fd->name());
    mFieldNamesToFds[p_fd->name()] = p_fd;
  }
}

std::vector<std::shared_ptr<google::protobuf::Message> > BaseModel::listMessage()
{
  // if we initialize the query with the query string, it will execute immediate and
  // we can't capture the error message
  QSqlQuery query(mpConnection->database());
  query.prepare("SELECT " + mFieldNames.join(',') + " FROM " + tableQ());

  std::vector<std::shared_ptr<google::protobuf::Message> > results;
  if (query.exec())
  {
    results.reserve(query.size());

    // gather all of the column names returned from the query
    const QSqlRecord record = query.record();
    const int column_count = record.count();
    std::vector<std::string> col_names;
    col_names.reserve(column_count);
    for (int c = 0; c < column_count; ++c)
    {
      col_names.push_back(record.fieldName(c).toStdString());
    }

    // iterate over every row the query returned
    while (query.next())
    {
      // create a new protobuf (generically, from our prototype)
      std::shared_ptr<google::protobuf::Message> p_msg(mpPrototype->New());
      // set fields on the message for each column returned in the query
      // note: we assume the column names match the message names
      for (int c = 0; c < column_count; ++c)
      {
        const std::string& col_name = col_names.at(c);
        const google::protobuf::FieldDescriptor* p_fd = mFieldNamesToFds[col_name];
        setFieldVariant(p_msg.get(), p_fd, query.value(c));
      }
      results.push_back(p_msg);
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

bool BaseModel::setFieldVariant(google::protobuf::Message* pMessage,
                                const google::protobuf::FieldDescriptor* pField,
                                const QVariant& var)
{
  switch (pField->cpp_type())
  {
    // for the int types, use the width based type names because its less confusing
    case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
      mpReflection->SetInt32(pMessage, pField, var.value<std::int32_t>());
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
      mpReflection->SetInt64(pMessage, pField, var.value<std::int64_t>());
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
      mpReflection->SetUInt32(pMessage, pField, var.value<std::uint32_t>());
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
      mpReflection->SetUInt64(pMessage, pField, var.value<std::uint64_t>());
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
      mpReflection->SetDouble(pMessage, pField, var.toFloat());
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
      mpReflection->SetFloat(pMessage, pField, var.toFloat());
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
      mpReflection->SetBool(pMessage, pField, var.toBool());
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
      mpReflection->SetEnumValue(pMessage, pField, var.toInt());
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
      mpReflection->SetString(pMessage, pField, var.toString().toStdString());
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

AUTOXTIME_DB_NAMESPACE_END
