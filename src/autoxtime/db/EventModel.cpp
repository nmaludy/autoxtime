#include <autoxtime/db/EventModel.h>

#include <autoxtime/proto/event.pb.h>

AUTOXTIME_DB_NAMESPACE_BEG

const std::string EventModel::TABLE = "event";
const std::string EventModel::PRIMARY_KEY = "event_id";

EventModel::EventModel(QObject* pParent)
    : EventModel(std::shared_ptr<DbConnection>(), pParent)
{}

EventModel::EventModel(std::shared_ptr<DbConnection> pConnection,
                       QObject* pParent)
    : BaseModelT(EventModel::TABLE,
                 EventModel::PRIMARY_KEY,
                 autoxtime::proto::Event::GetDescriptor(),
                 autoxtime::proto::Event::GetReflection(),
                 pConnection,
                 pParent)
{}

void EventModel
::emitSignal(EventModel::Signal signal,
             const std::vector<std::shared_ptr<autoxtime::proto::Event> >& protoList)
{
  switch (signal)
  {
    case SIGNAL_LIST_RESULT:
      emit listResult(protoList);
      break;
    case SIGNAL_CREATE_RESULT:
      emit createResult(protoList);
      break;
    case SIGNAL_UPDATE_RESULT:
      emit updateResult(protoList);
      break;
    case SIGNAL_FIND_RESULT:
      emit findResult(protoList);
      break;
  }
}

AUTOXTIME_DB_NAMESPACE_END
