#include <autoxtime/db/BaseModel.h>

// autoxtime
#include <autoxtime/db/DbConnection.h>
#include <autoxtime/db/DbListener.h>
#include <autoxtime/log/Log.h>

// protobuf
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

AUTOXTIME_DB_NAMESPACE_BEG

BaseModel::BaseModel(const std::string& table,
                     const std::string& primaryKey,
                     const google::protobuf::Descriptor* pDescriptor,
                     const google::protobuf::Reflection* pReflection,
                     std::shared_ptr<DbConnection> pConnection,
                     QObject* pParent)
    : QObject(pParent),
      mTable(table),
      mTableQ(QString::fromStdString(table)),
      mPrimaryKey(primaryKey),
      mPrimaryKeyQ(QString::fromStdString(primaryKey)),
      mpDescriptor(pDescriptor),
      mpReflection(pReflection),
      mpPrototype(pReflection->GetMessageFactory()->GetPrototype(mpDescriptor)),
      mpConnection(pConnection ? pConnection : std::make_shared<DbConnection>(this)),
      mFieldNames(fieldNames(mpDescriptor)),
      mFieldDescriptors(fieldDescriptors(mpDescriptor)),
      mFieldNamesToFds(fieldNamesToFds(mpDescriptor))
{}

QStringList BaseModel::fieldNames(const google::protobuf::Descriptor* pDescriptor)
{
  QStringList field_names;
  // cache the mapping of field names to descriptors and the field name list
  // this will traverse the fields in the order they are defined in the .proto file
  int field_count = pDescriptor->field_count();
  for (int i = 0; i < field_count; ++i)
  {
    const google::protobuf::FieldDescriptor* p_fd = pDescriptor->field(i);
    field_names << QString::fromStdString(p_fd->name());
  }
  return field_names;
}

std::vector<const google::protobuf::FieldDescriptor*> BaseModel
::fieldDescriptors(const google::protobuf::Descriptor* pDescriptor)
{
  int field_count = pDescriptor->field_count();
  std::vector<const google::protobuf::FieldDescriptor*> field_descriptors;
  field_descriptors.reserve(field_count);
  for (int i = 0; i < field_count; ++i)
  {
    const google::protobuf::FieldDescriptor* p_fd = pDescriptor->field(i);
    field_descriptors.push_back(p_fd);
  }
  return field_descriptors;
}


std::unordered_map<std::string, const google::protobuf::FieldDescriptor*> BaseModel
::fieldNamesToFds(const google::protobuf::Descriptor* pDescriptor)
{
  int field_count = pDescriptor->field_count();
  std::unordered_map<std::string, const google::protobuf::FieldDescriptor*> names_to_fds;
  names_to_fds.reserve(field_count);
  for (int i = 0; i < field_count; ++i)
  {
    const google::protobuf::FieldDescriptor* p_fd = pDescriptor->field(i);
    names_to_fds[p_fd->name()] = p_fd;
  }
  return names_to_fds;
}

void BaseModel::subscribeToNotifications()
{
  autoxtime::db::DbListener& listener = autoxtime::db::DbListener::instance();
  DbEmitter* p_emitter = listener.emitter(*mpPrototype, tableQ());
  connect(p_emitter, &DbEmitter::notification,
          this,      &BaseModel::emitNotification);
  listener.subscribe(*mpPrototype, tableQ());
}

void BaseModel::emitNotification(const std::shared_ptr<google::protobuf::Message>& pMessage,
                                 const QDateTime& timestamp,
                                 const QString& operation)
{
  AXT_DEBUG << "Base Model  " << table() << " emitting notification";
  emit notification(pMessage, timestamp, operation);
}

std::vector<std::shared_ptr<google::protobuf::Message> > BaseModel::listMessage()
{
  // just do a find with an empty message (our prototype)
  return findMessage(*mpPrototype);
}

std::vector<std::shared_ptr<google::protobuf::Message> > BaseModel
::createMessage(const google::protobuf::Message* pMessage)
{
  return createOrUpdateMessage(pMessage, true);
}

std::vector<std::shared_ptr<google::protobuf::Message> > BaseModel
::updateMessage(const google::protobuf::Message* pMessage)
{
  return createOrUpdateMessage(pMessage, false);
}

std::vector<std::shared_ptr<google::protobuf::Message> > BaseModel
::createOrUpdateMessage(const google::protobuf::Message* pMessage, bool bCreate)
{
  QElapsedTimer timer;
  timer.start();

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
                  + " VALUES (:" + fields.join(", :") + ")"
                  + " RETURNING " + primaryKeyQ());
  }
  else
  {
    // update is only different in the way it formats the query
    query.prepare("UPDATE " + tableQ() + " SET (" + fields.join(", ") + ")"
                  + " = (:" + fields.join(", :") + ")"
                  + " WHERE " + primaryKeyQ() + " = :" + primaryKeyQ()
                  + " RETURNING " + primaryKeyQ());
    // bind the primary key, only for updates
    if (p_fd_pk != nullptr)
    {
      query.bindValue(QString::fromStdString(":" + p_fd_pk->name()),
                      getFieldVariant(*pMessage, p_fd_pk));
    }
  }

  // bind only the fields that are set
  // this must occur after we call prepare()
  QString bound_values;
  QTextStream stream(&bound_values);
  for (const google::protobuf::FieldDescriptor* p_fd : set_fds)
  {
    QString field = QString::fromStdString(":" + p_fd->name());
    QVariant value = getFieldVariant(*pMessage, p_fd);
    query.bindValue(field, value);
    stream << "    " << field << " = " << value.toString() << "\n";
  }

  AXT_DEBUG
      << "BaseMode::createOrUpdate() executing query: " << query.lastQuery()
      << "\n" << bound_values;

  // execute query and check for success
  std::vector<std::shared_ptr<google::protobuf::Message> > results;
  if (query.exec())
  {
    results = parseQueryResults(query);
  }
  else
  {
    // TODO what in case of an error?
    AXT_ERROR << "Error executing query '" << query.lastQuery() << "' - "
              << query.lastError().text();
  }
  AXT_DEBUG
      << "BaseMode::createOrUpdate() done query: " << query.lastQuery()
      << " [" << timer.nsecsElapsed()/1.0e6 << "ms]"
      << "\n" << bound_values;
  return results;
}

std::int64_t BaseModel::destroyById(std::int64_t id)
{
  QElapsedTimer timer;
  timer.start();

  QSqlQuery query(connection()->database());
  const QString& pkey = primaryKeyQ();
  query.prepare("DELETE FROM " + tableQ() + " WHERE " + pkey + " = :" + pkey);
  query.bindValue(":" + pkey, QVariant::fromValue(id));
  AXT_DEBUG << "BaseMode::destroyById() executing query: " << query.lastQuery()
            << "\n" << ":" + pkey << "=" << id;
  bool res = query.exec();
  if (!res)
  {
    AXT_ERROR << "BaseModel::destroyById() - "
              << "Error executing query " << query.lastQuery() << " - "
              << query.lastError().text();
  }
  AXT_DEBUG << "BaseMode::destroyById() done query: " << query.lastQuery()
            << " [" << timer.nsecsElapsed()/1.0e6 << "ms]";
  return query.numRowsAffected();
}

std::vector<std::shared_ptr<google::protobuf::Message> > BaseModel
::findMessage(const google::protobuf::Message& prototype)
{
  QElapsedTimer timer;
  timer.start();

  // if we initialize the query with the query string, it will execute immediate and
  // we can't capture the error message
  QSqlQuery query(connection()->database());
  QString query_str( "SELECT " + mFieldNames.join(", ") + " FROM " + tableQ());

  // create a where clause
  // TODO should probably parameterize this
  QString where = wherePrototype(prototype);
  if (!where.isEmpty())
  {
    query_str += " WHERE " + where;
  }

  // prepare our query
  query.prepare(query_str);
  AXT_DEBUG << "BaseMode::findMessage() executing query: " << query.lastQuery();

  std::vector<std::shared_ptr<google::protobuf::Message> > results;
  if (query.exec())
  {
    results = parseQueryResults(query);
  }
  else
  {
    // TODO what in case of an error?
    AXT_ERROR << "Error executing query '" << query.lastQuery() << "' - "
              << query.lastError().text();
  }
  AXT_DEBUG << "BaseMode::findMessage() done query: " << query.lastQuery()
            << " [" << timer.nsecsElapsed()/1.0e6 << "ms]";
  return results;
}

std::vector<std::shared_ptr<google::protobuf::Message> > BaseModel
::findMessageById(std::int64_t id)
{
  QElapsedTimer timer;
  timer.start();

  QSqlQuery query(connection()->database());
  const QString& pkey = primaryKeyQ();
  query.prepare("SELECT " + mFieldNames.join(", ") + " FROM " + tableQ()
                + " WHERE " + pkey + " = :" + pkey);
  query.bindValue(":" + pkey, QVariant::fromValue(id));
  AXT_DEBUG << "BaseMode::findById() executing query: " << query.lastQuery()
            << "\n" << ":" + pkey << "=" << id;

  std::vector<std::shared_ptr<google::protobuf::Message> > results;
  if (query.exec())
  {
    results = parseQueryResults(query);
  }
  else
  {
    // TODO what in case of an error?
    AXT_ERROR << "Error executing query '" << query.lastQuery() << "' - "
              << query.lastError().text();
  }
  AXT_DEBUG << "BaseMode::findMessageById() done query: " << query.lastQuery()
            << " [" << timer.nsecsElapsed()/1.0e6 << "ms]";
  return results;
}

std::vector<std::shared_ptr<google::protobuf::Message> > BaseModel
::findMessageCustom(const QString& custom,
                    const std::unordered_map<QString, QVariant>& bindings)
{
  QElapsedTimer timer;
  timer.start();

  QSqlQuery query(connection()->database());
  query.prepare("SELECT " + tableQ() + ".* FROM " + tableQ()
                + " " + custom);
  for (const std::pair<const QString, QVariant>& bind : bindings)
  {
    AXT_DEBUG << "BaseMode::findMessageCustom() binding " << bind.first
              << " = " << bind.second;
    query.bindValue(bind.first, bind.second);
  }
  AXT_DEBUG << "BaseMode::findMessageCustom() executing query: " << query.lastQuery();

  std::vector<std::shared_ptr<google::protobuf::Message> > results;
  if (query.exec())
  {
    results = parseQueryResults(query);
  }
  else
  {
    // TODO what in case of an error?
    AXT_ERROR << "Error executing query '" << query.lastQuery() << "' - "
              << query.lastError().text();
  }
  AXT_DEBUG << "BaseMode::findMessageById() done query: " << query.lastQuery()
            << " [" << timer.nsecsElapsed()/1.0e6 << "ms]";
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
    // TODO make log levels configurable
    // AXT_DEBUG << "GOT QUERY RESULT";
    // create a new protobuf (generically, from our prototype)
    std::shared_ptr<google::protobuf::Message> p_msg(mpPrototype->New());
    // set fields on the message for each column returned in the query
    // note: we assume the column names match the message names
    for (int c = 0; c < column_count; ++c)
    {
      const std::string& col_name = col_names.at(c);
      // AXT_DEBUG << "GOT QUERY RESULT - column = " << col_name;
      const google::protobuf::FieldDescriptor* p_fd = mFieldNamesToFds.at(col_name);
      const QVariant& value = query.value(c);
      // AXT_DEBUG << "GOT QUERY RESULT - value = " << value.toString();
      setFieldVariant(p_msg.get(), p_fd, value);
    }
    results.push_back(p_msg);
  }
  return results;
}

QVariant BaseModel::getFieldVariant(const google::protobuf::Message& message,
                                    const google::protobuf::FieldDescriptor* pField)
{
  const google::protobuf::Reflection* p_reflection = message.GetReflection();
  switch (pField->cpp_type())
  {
    case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
      return QVariant(p_reflection->GetInt32(message, pField));
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
      return QVariant::fromValue<std::int64_t>(p_reflection->GetInt64(message, pField));
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
      return QVariant(p_reflection->GetUInt32(message, pField));
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
      return QVariant::fromValue<std::uint64_t>(p_reflection->GetUInt64(message, pField));
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
      return QVariant(p_reflection->GetDouble(message, pField));
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
      return QVariant(p_reflection->GetFloat(message, pField));
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
      return QVariant(p_reflection->GetBool(message, pField));
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
      return QVariant(p_reflection->GetEnumValue(message, pField));
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
      return QVariant(QString::fromStdString(p_reflection->GetString(message, pField)));
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
      AXT_ERROR << "Error getting field '"
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
  const google::protobuf::Reflection* p_reflection = pMessage->GetReflection();
  bool b_ok = false;
  switch (pField->cpp_type())
  {
    case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
      p_reflection->SetInt32(pMessage, pField, var.toInt(&b_ok));
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
      p_reflection->SetInt64(pMessage, pField, var.toLongLong(&b_ok));
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
      p_reflection->SetUInt32(pMessage, pField, var.toUInt(&b_ok));
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
      p_reflection->SetUInt64(pMessage, pField, var.toULongLong(&b_ok));
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
      p_reflection->SetDouble(pMessage, pField, var.toFloat(&b_ok));
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
      p_reflection->SetFloat(pMessage, pField, var.toFloat(&b_ok));
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
      p_reflection->SetBool(pMessage, pField, var.toBool());
      b_ok = true;
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
      p_reflection->SetEnumValue(pMessage, pField, var.toInt(&b_ok));
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
      p_reflection->SetString(pMessage, pField, var.toString().toStdString());
      b_ok = true;
      break;
    case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
      AXT_ERROR << "Error setting field '"
                << QString::fromStdString(pField->full_name())
                << "' "
                << "message types are not yet supported for parsing";
      return false;
      break;
  }

  if (!b_ok)
  {
    AXT_ERROR << "Error converting field '"
              << QString::fromStdString(pField->full_name())
              << "' from QVariant(string) to protobuf("
              << pField->type_name()
              << ") / c++("
              << pField->cpp_type_name()
              << ")  value = '" << var.toString().toStdString() << "'";
  }
  return b_ok;
}

QString BaseModel::wherePrototype(const google::protobuf::Message& message)
{
  const google::protobuf::Reflection* p_reflection = message.GetReflection();
  QStringList where_parts;
  for (const google::protobuf::FieldDescriptor* p_fd : mFieldDescriptors)
  {
    if (p_reflection->HasField(message, p_fd))
    {
      QString qfield = QString::fromStdString(p_fd->name());
      switch (p_fd->cpp_type())
      {
        case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
          where_parts << (qfield
                          + " = "
                          + QString("%1").arg(p_reflection->GetInt32(message, p_fd)));
          break;
        case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
          where_parts << (qfield
                          + " = "
                          + QString("%1").arg(p_reflection->GetInt64(message, p_fd)));
          break;
        case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
          where_parts << (qfield
                          + " = "
                          + QString("%1").arg(p_reflection->GetUInt32(message, p_fd)));
          break;
        case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
          where_parts << (qfield
                          + " = "
                          + QString("%1").arg(p_reflection->GetUInt64(message, p_fd)));
          break;
        case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
          where_parts << (qfield
                          + " = "
                          + QString("%1").arg(p_reflection->GetDouble(message, p_fd)));
          break;
        case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
          where_parts << (qfield
                          + " = "
                          + QString("%1").arg(p_reflection->GetFloat(message, p_fd)));
          break;
        case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
          where_parts << (qfield
                          + " = "
                          + QString("%1").arg(p_reflection->GetBool(message, p_fd)));
          break;
        case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
          where_parts << (qfield
                          + " = "
                          + QString("%1").arg(p_reflection->GetEnumValue(message, p_fd)));
          break;
        case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
          where_parts << (qfield
                          + " ILIKE '"
                          + QString::fromStdString(p_reflection->GetString(message, p_fd))
                          + "'");
          break;
        case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
          AXT_ERROR << "Error getting field '"
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
