#ifndef AUTOXTIME_DB_EVENTREGISTRATIONMODEL
#define AUTOXTIME_DB_EVENTREGISTRATIONMODEL

#include <autoxtime/db/db.h>
#include <autoxtime/db/BaseModelT.h>

namespace autoxtime { namespace proto { class EventRegistration; } }

AUTOXTIME_DB_NAMESPACE_BEG

class EventRegistrationModel : public BaseModelT<autoxtime::proto::EventRegistration, EventRegistrationModel>
{
  Q_OBJECT

 public:
  static const std::string TABLE;
  static const std::string PRIMARY_KEY;

  EventRegistrationModel(QObject* pParent = nullptr);
  EventRegistrationModel(std::shared_ptr<DbConnection> pConnection,
                         QObject* pParent = nullptr);

  // creates EventRegistration if doesn't exist, order of lookup is:
  // - event_id + driver_id + car_id
  ProtoPtrVec createIfNotExists(const autoxtime::proto::EventRegistration& eventRegistration);

  virtual void emitSignal(Signal signal,
                          const std::vector<std::shared_ptr<autoxtime::proto::EventRegistration> >& results);

 signals:
  // need the full namespace here so it matches the Q_DECLARE_METATYPE below
  void listResult(const std::vector<std::shared_ptr<autoxtime::proto::EventRegistration> >& protoList);
  void createResult(const std::vector<std::shared_ptr<autoxtime::proto::EventRegistration> >& protoList);
  void updateResult(const std::vector<std::shared_ptr<autoxtime::proto::EventRegistration> >& protoList);
  void findResult(const std::vector<std::shared_ptr<autoxtime::proto::EventRegistration> >& protoList);
};

AUTOXTIME_DB_NAMESPACE_END

// needed so we can use signals/slots with this type
Q_DECLARE_METATYPE(std::vector<std::shared_ptr<autoxtime::proto::EventRegistration>>)

#endif // AUTOXTIME_DB_EVENTREGISTRATION
