#include <autoxtime/db/BaseModel.h>

#include <autoxtime/db/DbConnection.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include <QtDebug>

AUTOXTIME_DB_NAMESPACE_BEG

BaseModel::BaseModel(const std::string& table,
                     const std::string& primaryKey,
                     const google::protobuf::Descriptor* pDescriptor,
                     const google::protobuf::Reflection* pReflection,
                     std::shared_ptr<DbConnection> pConnection)
    : QObject(),
      mTable(table),
      mTableQ(QString::fromStdString(table)),
      mPrimaryKey(primaryKey),
      mPrimaryKeyQ(QString::fromStdString(primaryKey)),
      mpDescriptor(pDescriptor),
      mpReflection(pReflection),
      mpPrototype(pReflection->GetMessageFactory()->GetPrototype(mpDescriptor)),
      mpConnection(pConnection ? pConnection : std::make_shared<DbConnection>()),
      mFieldNames(),
      mFieldNamesToFds()
{
  // cache the mapping of field names to descriptors and the field name list
  // this will traverse the fields in the order they are defined in the .proto file
  int field_count = mpDescriptor->field_count();
  mFieldDescriptors.reserve(field_count);
  mFieldNamesToFds.reserve(field_count);
  for (int i = 0; i < field_count; ++i)
  {
    const google::protobuf::FieldDescriptor* p_fd = mpDescriptor->field(i);
    mFieldNames << QString::fromStdString(p_fd->name());
    mFieldNamesToFds[p_fd->name()] = p_fd;
    mFieldDescriptors.push_back(p_fd);
  }
}

std::vector<std::shared_ptr<google::protobuf::Message> > BaseModel::listMessage()
{
  // just do a find with an empty message (our prototype)
  return findMessage(*mpPrototype);
}

int BaseModel::createMessage(const google::protobuf::Message* pMessage)
{
  return createOrUpdateMessage(pMessage, true);
}

int BaseModel::updateMessage(const google::protobuf::Message* pMessage)
{
  return createOrUpdateMessage(pMessage, false);
}

int BaseModel::createOrUpdateMessage(const google::protobuf::Message* pMessage, bool bCreate)
{
  QSqlQuery query(connection()->database());
  QStringList fields;
  std::vector<const google::protobuf::FieldDescriptor*> set_fds;
  set_fds.reserve(mFieldDescriptors.size());
  const google::protobuf::FieldDescriptor* p_fd_pk = nullptr;
  for (const google::protobuf::FieldDescriptor* p_fd : mFieldDescriptors)
  {
    // skip the primary key field 
    if (p_fd->name() == primaryKey())
    {
      p_fd_pk = p_fd;
      continue;
    }
    
    if (mpReflection->HasField(*pMessage, p_fd))
    {
      fields << QString::fromStdString(p_fd->name());
      set_fds.push_back(p_fd);
    }
  }

  // create vs update are only different in the query syntax, the way we do
  // fields and bindings are the same, kind of nice to share some code
  if (bCreate)
  {
    // create our INSERT query with the data from the message as bindings
    query.prepare("INSERT INTO " + tableQ() + " (" + fields.join(", ") + ")"
                  + " VALUES (:" + fields.join(", :") + ")");
  }
  else
  {
    // update is only different in the way it formats the query
    query.prepare("UPDATE " + tableQ() + " SET (" + fields.join(", ") + ")"
                  + " = (:" + fields.join(", :") + ")"
                  + "WHERE " + primaryKeyQ() + " = :" + primaryKeyQ());
    // bind the primary key, only for updates
    if (p_fd_pk != nullptr)
    {
      query.bindValue(QString::fromStdString(":" + p_fd_pk->name()),
                      getFieldVariant(*pMessage, p_fd_pk));
    }
  }

  // bind only the fields that are set
  // this must occur after we call prepare()
  for (const google::protobuf::FieldDescriptor* p_fd : set_fds)
  {
    query.bindValue(QString::fromStdString(":" + p_fd->name()),
                    getFieldVariant(*pMessage, p_fd));
  }

  qDebug().nospace() << "BaseMode::createOrUpdate() executing query: " << query.lastQuery();
  
  // execute query and check for success
  if (!query.exec())
  {
    // TODO what in case of an error?
    qCritical().nospace() << "Error executing query '" << query.lastQuery() << "' - "
                          << query.lastError().text();
    return -1;
  }
  return query.numRowsAffected();
}

int BaseModel::destroyById(int id)
{
  QSqlQuery query(connection()->database());
  const QString& pkey = primaryKeyQ();
  query.prepare("DELETE FROM " + tableQ() + " WHERE " + pkey + " = :" + pkey);
  query.bindValue(":" + pkey, id);
  qDebug().nospace() << "BaseMode::destroyById() executing query: " << query.lastQuery();
  bool res = query.exec();
  if (!res)
  {
    qCritical().nospace() << "BaseModel::destroyById() - "
                          << "Error executing query " << query.lastQuery() << " - "
                          << query.lastError().text();
  }
  return query.numRowsAffected();
}

std::vector<std::shared_ptr<google::protobuf::Message> > BaseModel
::findMessage(const google::protobuf::Message& prototype)
{
  // if we initialize the query with the query string, it will execute immediate and
  // we can't capture the error message
  QSqlQuery query(connection()->database());
  QString query_str( "SELECT " + mFieldNames.join(", ") + " FROM " + tableQ());

  // create a where clause
  QString where = wherePrototype(prototype);
  if (!where.isEmpty())
  {
    query_str += " WHERE " + where;
  }

  // prepare our query
  query.prepare(query_str);
  qDebug().nospace() << "BaseMode::findMessage() executing query: " << query.lastQuery();

  std::vector<std::shared_ptr<google::protobuf::Message> > results; 
  if (query.exec())
  {
    results = parseQueryResults(query);
  }
  else
  {
    // TODO what in case of an error?
    qCritical().nospace() << "Error executing query '" << query.lastQuery() << "' - "
                          << query.lastError().text();
  }
  return results;
}

std::vector<std::shared_ptr<google::protobuf::Message> > BaseModel
::findMessageById(int id)
{
  QSqlQuery query(connection()->database());
  const QString& pkey = primaryKeyQ();
  query.prepare("SELECT " + mFieldNames.join(", ") + " FROM " + tableQ()
                + " WHERE " + pkey + " = :" + pkey);
  query.bindValue(":" + pkey, id);
  qDebug().nospace() << "BaseMode::findById() executing query: " << query.lastQuery();
  
  std::vector<std::shared_ptr<google::protobuf::Message> > results; 
  if (query.exec())
  {
    results = parseQueryResults(query);
  }
  else
  {
    // TODO what in case of an error?
    qCritical().nospace() << "Error executing query '" << query.lastQuery() << "' - "
                          << query.lastError().text();
  }
  return results;
}

std::vector<std::shared_ptr<google::protobuf::Message> > BaseModel
::parseQueryResults(QSqlQuery& query)
{
  std::vector<std::shared_ptr<google::protobuf::Message> > results;
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
  return results;
}

QVariant BaseModel::getFieldVariant(const google::protobuf::Message& message,
                                    const google::protobuf::FieldDescriptor* pField)
{
  switch (pField->cpp_type())
  {
    case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
      return QVariant(mpReflection->GetInt32(message, pField));
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
      return QVariant::fromValue<std::int64_t>(mpReflection->GetInt64(message, pField));
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
      return QVariant(mpReflection->GetUInt32(message, pField));
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
      return QVariant::fromValue<std::uint64_t>(mpReflection->GetUInt64(message, pField));
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
      return QVariant(mpReflection->GetDouble(message, pField));
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
      return QVariant(mpReflection->GetFloat(message, pField));
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
      return QVariant(mpReflection->GetBool(message, pField));
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
      return QVariant(mpReflection->GetEnumValue(message, pField));
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
      return QVariant(QString::fromStdString(mpReflection->GetString(message, pField)));
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
      qCritical().nospace() << "Error getting field '"
                            << QString::fromStdString(pField->full_name())
                            << "' "
                            << "message types are not yet supported for parsing";
      break;
  }
  return QVariant();
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

QString BaseModel::wherePrototype(const google::protobuf::Message& message)
{
  QStringList where_parts;
  for (const google::protobuf::FieldDescriptor* p_fd : mFieldDescriptors)
  {    
    if (mpReflection->HasField(message, p_fd))
    {
      QString qfield = QString::fromStdString(p_fd->name());
      switch (p_fd->cpp_type())
      {
        case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
          where_parts << (qfield
                          + " = "
                          + QString("%1").arg(mpReflection->GetInt32(message, p_fd)));
          break;
        case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
          where_parts << (qfield
                          + " = "
                          + QString("%1").arg(mpReflection->GetInt64(message, p_fd)));
          break;
        case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
          where_parts << (qfield
                          + " = "
                          + QString("%1").arg(mpReflection->GetUInt32(message, p_fd)));
          break;
        case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
          where_parts << (qfield
                          + " = "
                          + QString("%1").arg(mpReflection->GetUInt64(message, p_fd)));
          break;
        case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
          where_parts << (qfield
                          + " = "
                          + QString("%1").arg(mpReflection->GetDouble(message, p_fd)));
          break;
        case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
          where_parts << (qfield
                          + " = "
                          + QString("%1").arg(mpReflection->GetFloat(message, p_fd)));
          break;
        case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
          where_parts << (qfield
                          + " = "
                          + QString("%1").arg(mpReflection->GetBool(message, p_fd)));
          break;
        case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
          where_parts << (qfield
                          + " = "
                          + QString("%1").arg(mpReflection->GetEnumValue(message, p_fd)));
          break;
        case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
          where_parts << (qfield
                          + " ILIKE '"
                          + QString::fromStdString(mpReflection->GetString(message, p_fd))
                          + "'");
          break;
        case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
          qCritical().nospace() << "Error getting field '"
                                << QString::fromStdString(p_fd->full_name())
                                << "' "
                                << "message types are not yet supported for parsing";
          break;
      }      
    }
  }
  return where_parts.join(" AND ");
}

AUTOXTIME_DB_NAMESPACE_END
