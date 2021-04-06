#ifndef AUTOXTIME_DB_EVENTREGISTRATIONMODEL
#define AUTOXTIME_DB_EVENTREGISTRATIONMODEL

#include <autoxtime/db/db.h>
#include <autoxtime/db/BaseModel.h>

namespace autoxtime::proto { class EventRegistration; }

AUTOXTIME_DB_NAMESPACE_BEG

class EventRegistrationModel : public BaseModel
{
  Q_OBJECT;

 public:
  static const std::string TABLE;
  static const std::string PRIMARY_KEY;

  EventRegistrationModel(QObject* pParent = nullptr);
  EventRegistrationModel(std::shared_ptr<DbConnection> pConnection,
                QObject* pParent = nullptr);

  std::vector<std::shared_ptr<autoxtime::proto::EventRegistration> > list();
  std::vector<std::shared_ptr<autoxtime::proto::EventRegistration> > create(const autoxtime::proto::EventRegistration& eventRegistration);
  std::vector<std::shared_ptr<autoxtime::proto::EventRegistration> > update(const autoxtime::proto::EventRegistration& eventRegistration);

  std::vector<std::shared_ptr<autoxtime::proto::EventRegistration> > find(const autoxtime::proto::EventRegistration& prototype);
  std::vector<std::shared_ptr<autoxtime::proto::EventRegistration> > findById(int id);

  // creates EventRegistration if doesn't exist, order of lookup is:
  // - event_id + driver_id + car_id
  std::vector<std::shared_ptr<autoxtime::proto::EventRegistration> > createIfNotExists(const autoxtime::proto::EventRegistration& eventRegistration);
};

AUTOXTIME_DB_NAMESPACE_END

#endif // AUTOXTIME_DB_EVENTREGISTRATION
