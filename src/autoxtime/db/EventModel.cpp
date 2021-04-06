#include <autoxtime/db/EventModel.h>

#include <autoxtime/db/DbConnection.h>
#include <autoxtime/proto/event.pb.h>

#include <QtDebug>
#include <QtConcurrent/QtConcurrent>

AUTOXTIME_DB_NAMESPACE_BEG

const std::string EventModel::TABLE = "event";
const std::string EventModel::PRIMARY_KEY = "event_id";

EventModel::EventModel(QObject* pParent)
    : EventModel(std::shared_ptr<DbConnection>(), pParent)
{}

EventModel::EventModel(std::shared_ptr<DbConnection> pConnection,
                       QObject* pParent)
    : BaseModel(EventModel::TABLE,
                EventModel::PRIMARY_KEY,
                autoxtime::proto::Event::GetDescriptor(),
                autoxtime::proto::Event::GetReflection(),
                pConnection,
                pParent)
{
  qRegisterMetaType<EventModel::ProtoPtrVec>();
}

EventModel::ProtoPtrVec EventModel::list()
{
  return BaseModel::listT<EventModel::Proto>();
}

QFuture<EventModel::ProtoPtrVec> EventModel::listAsync()
{
  return QtConcurrent::run([=]() {
    // Code in this block will run in another thread

    // TODO, can we avoid createing new models for each query?
    // right now we are doing this for thread safety, probably for the better (honestly)
    std::unique_ptr<EventModel> p_model = std::make_unique<EventModel>();
    EventModel::ProtoPtrVec results = p_model->list();
    emit listResult(results);
    return results;
  });
}

EventModel::ProtoPtrVec EventModel::create(const EventModel::Proto& event)
{
  return BaseModel::createT(event);
}

EventModel::ProtoPtrVec EventModel::update(const EventModel::Proto& event)
{
  return BaseModel::updateT(event);
}

EventModel::ProtoPtrVec EventModel::find(const EventModel::Proto& prototype)
{
  return BaseModel::findT<EventModel::Proto>(prototype);
}

EventModel::ProtoPtrVec EventModel::findById(std::int64_t id)
{
  return BaseModel::findByIdT<EventModel::Proto>(id);
}

AUTOXTIME_DB_NAMESPACE_END
