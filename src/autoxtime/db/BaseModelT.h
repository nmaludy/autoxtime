#ifndef AUTOXTIME_DB_BASEMODELT
#define AUTOXTIME_DB_BASEMODELT

#include <autoxtime/db/BaseModel.h>
#include <autoxtime/db/DbThreadPool.h>

#include <QtConcurrent/QtConcurrent>

AUTOXTIME_DB_NAMESPACE_BEG

template <typename T, typename M>
class BaseModelT : public BaseModel
{
 public:
  typedef T Proto;
  typedef M Model;
  typedef std::shared_ptr<Proto> ProtoPtr;
  typedef std::vector<ProtoPtr> ProtoPtrVec;

  enum Signal
  {
    SIGNAL_LIST_RESULT,
    SIGNAL_CREATE_RESULT,
    SIGNAL_UPDATE_RESULT,
    SIGNAL_FIND_RESULT,
  };

  BaseModelT(const std::string& table,
             const std::string& primaryKey,
             const google::protobuf::Descriptor* pDescriptor,
             const google::protobuf::Reflection* pReflection,
             std::shared_ptr<DbConnection> pConnection = std::shared_ptr<DbConnection>(),
             QObject* pParent = nullptr);
  virtual ~BaseModelT() = default;

  virtual void emitSignal(Signal signal,
                          const std::vector<std::shared_ptr<T> >& results) = 0;

  // conversion helpers
  static std::shared_ptr<T> messageToT(const std::shared_ptr<google::protobuf::Message>& msg);
  static std::vector<std::shared_ptr<T> > messagesToT(std::vector<std::shared_ptr<google::protobuf::Message> >& messages);

  // list
  std::vector<std::shared_ptr<T> > list();
  QFuture<std::vector<std::shared_ptr<T>>> listAsync();

  // create
  std::vector<std::shared_ptr<T> > create(const T* pMsg);
  std::vector<std::shared_ptr<T> > create(const T& msg);
  QFuture<std::vector<std::shared_ptr<T>>> createAsync(const T& msg);

  // update
  std::vector<std::shared_ptr<T> > update(const T* pMsg);
  std::vector<std::shared_ptr<T> > update(const T& msg);
  QFuture<std::vector<std::shared_ptr<T>>> updateAsync(const T& msg);

  // find
  std::vector<std::shared_ptr<T> > find(const T& prototype);
  QFuture<std::vector<std::shared_ptr<T>>> findAsync(const T& event);

  std::vector<std::shared_ptr<T> > findById(std::int64_t id);
  QFuture<std::vector<std::shared_ptr<T>>> findByIdAsync(std::int64_t id);

  std::vector<std::shared_ptr<T> > findCustom(const QString& custom,
                                              const std::unordered_map<QString, QVariant>& bindings = std::unordered_map<QString, QVariant>());
  QFuture<std::vector<std::shared_ptr<T>>> findCustomAsync(const QString& custom,
                                                           const std::unordered_map<QString, QVariant>& bindings = std::unordered_map<QString, QVariant>());
};

template <typename T, typename M>
BaseModelT<T, M>::BaseModelT(const std::string& table,
                             const std::string& primaryKey,
                             const google::protobuf::Descriptor* pDescriptor,
                             const google::protobuf::Reflection* pReflection,
                             std::shared_ptr<DbConnection> pConnection,
                             QObject* pParent)
    : BaseModel(table,
                primaryKey,
                pDescriptor,
                pReflection,
                pConnection,
                pParent)
{
  qRegisterMetaType<ProtoPtrVec>();
}

template <typename T, typename M>
std::shared_ptr<T> BaseModelT<T, M>
::messageToT(const std::shared_ptr<google::protobuf::Message>& msg)
{
  return std::dynamic_pointer_cast<T>(msg);
}

template <typename T, typename M>
std::vector<std::shared_ptr<T> > BaseModelT<T, M>
::messagesToT(std::vector<std::shared_ptr<google::protobuf::Message> >& messages)
{
  std::vector<std::shared_ptr<T> > results;
  results.reserve(messages.size());
  std::transform(messages.begin(), messages.end(), std::back_inserter(results),
                 BaseModelT::messageToT);
  return results;
}

/////////////////////////////////////////////////////////////////// list

template <typename T, typename M>
std::vector<std::shared_ptr<T>> BaseModelT<T, M>::list()
{
  std::vector<std::shared_ptr<google::protobuf::Message> > msgs = listMessage();
  return messagesToT(msgs);
}

template <typename T, typename M>
QFuture<std::vector<std::shared_ptr<T>>> BaseModelT<T, M>::listAsync()
{
  return QtConcurrent::run(DbThreadPool::instance(), [=]() {
    // create a new model so we have connection for this thread (required)
    M model;
    ProtoPtrVec results = model.list();
    emitSignal(SIGNAL_LIST_RESULT, results);
    return results;
  });
}

////////////////////////////////////////////////////////////////// create

template <typename T, typename M>
std::vector<std::shared_ptr<T> > BaseModelT<T, M>::create(const T* pMessage)
{
  const google::protobuf::Message* p_proto =
      dynamic_cast<const google::protobuf::Message*>(pMessage);
  std::vector<std::shared_ptr<google::protobuf::Message> > msgs = createMessage(p_proto);
  return messagesToT(msgs);
}

template <typename T, typename M>
std::vector<std::shared_ptr<T>> BaseModelT<T, M>::create(const T& msg)
{
  return create(&msg);
}

template <typename T, typename M>
QFuture<std::vector<std::shared_ptr<T>>> BaseModelT<T, M>::createAsync(const T& msg)
{
  return QtConcurrent::run(DbThreadPool::instance(), [=]() {
    // create a new model so we have connection for this thread (required)
    M model;
    ProtoPtrVec results = model.create(msg);
    emitSignal(SIGNAL_CREATE_RESULT, results);
    return results;
  });
}

template <typename T, typename M>
std::vector<std::shared_ptr<T> > BaseModelT<T, M>::update(const T* pMessage)
{
  const google::protobuf::Message* p_proto =
      dynamic_cast<const google::protobuf::Message*>(pMessage);
  std::vector<std::shared_ptr<google::protobuf::Message> > msgs = updateMessage(p_proto);
  return messagesToT(msgs);
}

/////////////////////////////////////////////////////////////////////// update

template <typename T, typename M>
std::vector<std::shared_ptr<T>> BaseModelT<T, M>::update(const T& msg)
{
  return update(&msg);
}

template <typename T, typename M>
QFuture<std::vector<std::shared_ptr<T>>> BaseModelT<T, M>::updateAsync(const T& msg)
{
  return QtConcurrent::run(DbThreadPool::instance(), [=]() {
    // create a new model so we have connection for this thread (required)
    M model;
    ProtoPtrVec results = model.update(msg);
    emitSignal(SIGNAL_UPDATE_RESULT, results);
    return results;
  });
}

////////////////////////////////////////////////////////////////////// find

template <typename T, typename M>
std::vector<std::shared_ptr<T>> BaseModelT<T, M>::find(const T& prototype)
{
  std::vector<std::shared_ptr<google::protobuf::Message> > msgs = findMessage(prototype);
  return messagesToT(msgs);
}

template <typename T, typename M>
QFuture<std::vector<std::shared_ptr<T>>> BaseModelT<T, M>::findAsync(const T& msg)
{
  return QtConcurrent::run(DbThreadPool::instance(), [=]() {
    // create a new model so we have connection for this thread (required)
    M model;
    ProtoPtrVec results = model.find(msg);
    emitSignal(SIGNAL_FIND_RESULT, results);
    return results;
  });
}

template <typename T, typename M>
std::vector<std::shared_ptr<T>> BaseModelT<T, M>::findById(std::int64_t id)
{
  std::vector<std::shared_ptr<google::protobuf::Message> > msgs = findMessageById(id);
  return messagesToT(msgs);
}

template <typename T, typename M>
QFuture<std::vector<std::shared_ptr<T>>> BaseModelT<T, M>::findByIdAsync(std::int64_t id)
{
  return QtConcurrent::run(DbThreadPool::instance(), [=]() {
    // create a new model so we have connection for this thread (required)
    M model;
    ProtoPtrVec results = model.findById(id);
    emitSignal(SIGNAL_FIND_RESULT, results);
    return results;
  });
}

template <typename T, typename M>
std::vector<std::shared_ptr<T>> BaseModelT<T, M>
::findCustom(const QString& custom,
             const std::unordered_map<QString, QVariant>& bindings)
{
  std::vector<std::shared_ptr<google::protobuf::Message> > msgs = findMessageCustom(custom, bindings);
  return messagesToT(msgs);
}

template <typename T, typename M>
QFuture<std::vector<std::shared_ptr<T>>> BaseModelT<T, M>
::findCustomAsync(const QString& custom,
                  const std::unordered_map<QString, QVariant>& bindings)
{
  return QtConcurrent::run(DbThreadPool::instance(), [=]() {
    // create a new model so we have connection for this thread (required)
    M model;
    ProtoPtrVec results = model.findCustom(custom, bindings);
    emitSignal(SIGNAL_FIND_RESULT, results);
    return results;
  });
}

AUTOXTIME_DB_NAMESPACE_END

#endif // AUTOXTIME_DB_BASEMODELT
