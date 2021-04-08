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
    // create a new model so we have connection for this thread (required)
    EventModel model;
    EventModel::ProtoPtrVec results = model.list();
    emit listResult(results);
    return results;
  });
}

EventModel::ProtoPtrVec EventModel::create(const EventModel::Proto& event)
{
  return BaseModel::createT(event);
}

QFuture<EventModel::ProtoPtrVec> EventModel::createAsync(const EventModel::Proto& event)
{
  // copy before transitioning to thread so we don't have dual memory access
  EventModel::Proto event_copy(event);
  return QtConcurrent::run([=]() {
    // create a new model so we have connection for this thread (required)
    EventModel model;
    EventModel::ProtoPtrVec results = model.create(event_copy);
    emit createResult(results);
    return results;
  });
}

EventModel::ProtoPtrVec EventModel::update(const EventModel::Proto& event)
{
  return BaseModel::updateT(event);
}

QFuture<EventModel::ProtoPtrVec> EventModel::updateAsync(const EventModel::Proto& event)
{
  // copy before transitioning to thread so we don't have dual memory access
  EventModel::Proto event_copy(event);
  return QtConcurrent::run([=]() {
    // create a new model so we have connection for this thread (required)
    EventModel model;
    EventModel::ProtoPtrVec results = model.update(event_copy);
    emit updateResult(results);
    return results;
  });
}

EventModel::ProtoPtrVec EventModel::find(const EventModel::Proto& prototype)
{
  return BaseModel::findT<EventModel::Proto>(prototype);
}

EventModel::ProtoPtrVec EventModel::findById(std::int64_t id)
{
  return BaseModel::findByIdT<EventModel::Proto>(id);
}

QFuture<EventModel::ProtoPtrVec> EventModel::findAsync(const EventModel::Proto& event)
{
  // copy before transitioning to thread so we don't have dual memory access
  EventModel::Proto event_copy(event);
  return QtConcurrent::run([=]() {
    // create a new model so we have connection for this thread (required)
    EventModel model;
    EventModel::ProtoPtrVec results = model.find(event_copy);
    emit findResult(results);
    return results;
  });
}

AUTOXTIME_DB_NAMESPACE_END
