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

  std::vector<std::shared_ptr<T> > list();
  QFuture<std::vector<std::shared_ptr<T>>> listAsync();

  std::vector<std::shared_ptr<T> > create(const T& msg);
  QFuture<std::vector<std::shared_ptr<T>>> createAsync(const T& msg);

  std::vector<std::shared_ptr<T> > update(const T& msg);
  QFuture<std::vector<std::shared_ptr<T>>> updateAsync(const T& msg);

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
std::vector<std::shared_ptr<T>> BaseModelT<T, M>::list()
{
  return BaseModel::listT<T>();
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

template <typename T, typename M>
std::vector<std::shared_ptr<T>> BaseModelT<T, M>::create(const T& msg)
{
  return BaseModel::createT(msg);
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
std::vector<std::shared_ptr<T>> BaseModelT<T, M>::update(const T& msg)
{
  return BaseModel::updateT(msg);
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

template <typename T, typename M>
std::vector<std::shared_ptr<T>> BaseModelT<T, M>::find(const T& prototype)
{
  return BaseModel::findT<T>(prototype);
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
  return BaseModel::findByIdT<T>(id);
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
  return BaseModel::findCustomT<T>(custom, bindings);
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
