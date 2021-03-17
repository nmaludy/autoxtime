#ifndef AUTOXTIME_DB_ASYNCMODEL
#define AUTOXTIME_DB_ASYNCMODEL

#include <autoxtime/db/db.h>

#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <memory>

AUTOXTIME_DB_NAMESPACE_BEG

template <typename Model, typename Proto>
class AsyncModel : public QObject
{
  Q_OBJECT;

 public:
  AsyncModel(QObject* pParent = nullptr);

  QFuture<std::vector<std::shared_ptr<Proto> > > list();
  // int create(const autoxtime::proto::Driver& driver);
  // int update(const autoxtime::proto::Driver& driver);

  // std::vector<std::shared_ptr<autoxtime::proto::Driver> > find(const autoxtime::proto::Driver& prototype);
  // std::vector<std::shared_ptr<autoxtime::proto::Driver> > findById(int id);

 signals:
  void listResult(const std::vector<std::shared_ptr<Proto> >& protoList);
};

template <typename Model, typename Proto>
AsyncModel<Model, Proto>::AsyncModel(QObject* pParent)
    : QObject(pParent)
{}

template <typename Model, typename Proto>
QFuture<std::vector<std::shared_ptr<Proto> > > AsyncModel<Model, Proto>::list()
{
  return QtConcurrent::run([=]() {
    // Code in this block will run in another thread
    std::unique_ptr<Model> p_model = std::make_unique<Model>();
    std::vector<std::shared_ptr<Proto> > results = p_model->list();
    emit listResult(results);
    return results;
  });
}

AUTOXTIME_DB_NAMESPACE_END

#endif // AUTOXTIME_DB_ASYNCMODEL
