#include <autoxtime/db/EventRegistrationModel.h>

#include <autoxtime/db/DbConnection.h>
#include <autoxtime/proto/event_registration.pb.h>

#include <QtDebug>

AUTOXTIME_DB_NAMESPACE_BEG

const std::string EventRegistrationModel::TABLE = "event_registration";
const std::string EventRegistrationModel::PRIMARY_KEY = "event_registration_id";

EventRegistrationModel::EventRegistrationModel(QObject* pParent)
    : EventRegistrationModel(std::shared_ptr<DbConnection>(), pParent)
{}

EventRegistrationModel::EventRegistrationModel(std::shared_ptr<DbConnection> pConnection,
                         QObject* pParent)
    : BaseModel(EventRegistrationModel::TABLE,
                EventRegistrationModel::PRIMARY_KEY,
                autoxtime::proto::EventRegistration::GetDescriptor(),
                autoxtime::proto::EventRegistration::GetReflection(),
                pConnection,
                pParent)
{}

std::vector<std::shared_ptr<autoxtime::proto::EventRegistration> > EventRegistrationModel::list()
{
  return BaseModel::listT<autoxtime::proto::EventRegistration>();
}

std::vector<std::shared_ptr<autoxtime::proto::EventRegistration> > EventRegistrationModel
::create(const autoxtime::proto::EventRegistration& eventRegistration)
{
  return BaseModel::createT(eventRegistration);
}

std::vector<std::shared_ptr<autoxtime::proto::EventRegistration> > EventRegistrationModel
::update(const autoxtime::proto::EventRegistration& eventRegistration)
{
  return BaseModel::updateT(eventRegistration);
}

std::vector<std::shared_ptr<autoxtime::proto::EventRegistration> > EventRegistrationModel
::find(const autoxtime::proto::EventRegistration& prototype)
{
  return BaseModel::findT<autoxtime::proto::EventRegistration>(prototype);
}

std::vector<std::shared_ptr<autoxtime::proto::EventRegistration> > EventRegistrationModel
::findById(std::int64_t id)
{
  return BaseModel::findByIdT<autoxtime::proto::EventRegistration>(id);
}

std::vector<std::shared_ptr<autoxtime::proto::EventRegistration> > EventRegistrationModel
::createIfNotExists(const autoxtime::proto::EventRegistration& eventRegistration)
{
  // creates eventRegistration if doesn't exist, order of lookup is:
  // - model + color + class_id + driver_id
  std::vector<std::shared_ptr<autoxtime::proto::EventRegistration> > results;

  // primary key
  if (eventRegistration.has_event_registration_id())
  {
    autoxtime::proto::EventRegistration proto;
    proto.set_event_registration_id(eventRegistration.event_registration_id());
    results = find(proto);
    if (!results.empty())
    {
      return results;
    }
  }

  // event_id + driver_id + car_id
  if (eventRegistration.has_event_id() &&
      eventRegistration.has_driver_id() &&
      eventRegistration.has_car_id())
  {
    autoxtime::proto::EventRegistration proto;
    proto.set_event_id(eventRegistration.event_id());
    proto.set_driver_id(eventRegistration.driver_id());
    proto.set_car_id(eventRegistration.car_id());

    results = find(proto);
    if (!results.empty())
    {
      return results;
    }
  }

  // create if we haven't found it yet
  if (results.empty())
  {
    results = create(eventRegistration);
  }
  return results;
}

AUTOXTIME_DB_NAMESPACE_END
