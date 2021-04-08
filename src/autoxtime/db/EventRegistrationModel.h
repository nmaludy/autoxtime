#ifndef AUTOXTIME_DB_EVENTREGISTRATIONMODEL
#define AUTOXTIME_DB_EVENTREGISTRATIONMODEL

#include <autoxtime/db/db.h>
#include <autoxtime/db/BaseModel.h>

namespace autoxtime { namespace proto { class EventRegistration; } }

AUTOXTIME_DB_NAMESPACE_BEG

class EventRegistrationModel : public BaseModel
{
  Q_OBJECT

 public:
  static const std::string TABLE;
  static const std::string PRIMARY_KEY;

  typedef autoxtime::proto::EventRegistration Proto;
  typedef std::shared_ptr<Proto> ProtoPtr;
  typedef std::vector<ProtoPtr> ProtoPtrVec;

  EventRegistrationModel(QObject* pParent = nullptr);
  EventRegistrationModel(std::shared_ptr<DbConnection> pConnection,
                         QObject* pParent = nullptr);

  ProtoPtrVec list();
  QFuture<ProtoPtrVec> listAsync();

  ProtoPtrVec create(const autoxtime::proto::EventRegistration& eventRegistration);
  QFuture<ProtoPtrVec> createAsync(const Proto& event);

  ProtoPtrVec update(const autoxtime::proto::EventRegistration& eventRegistration);
  QFuture<ProtoPtrVec> updateAsync(const Proto& event);

  ProtoPtrVec find(const autoxtime::proto::EventRegistration& prototype);
  QFuture<ProtoPtrVec> findAsync(const Proto& event);
  ProtoPtrVec findById(std::int64_t id);

  // creates EventRegistration if doesn't exist, order of lookup is:
  // - event_id + driver_id + car_id
  ProtoPtrVec createIfNotExists(const autoxtime::proto::EventRegistration& eventRegistration);

 signals:
  // need the full namespace here so it matches the Q_DECLARE_METATYPE below
  void listResult(const autoxtime::db::EventRegistrationModel::ProtoPtrVec& protoList);
  void createResult(const autoxtime::db::EventRegistrationModel::ProtoPtrVec& protoList);
  void updateResult(const autoxtime::db::EventRegistrationModel::ProtoPtrVec& protoList);
  void findResult(const autoxtime::db::EventRegistrationModel::ProtoPtrVec& protoList);
};

AUTOXTIME_DB_NAMESPACE_END

// needed so we can use signals/slots with this type
Q_DECLARE_METATYPE(autoxtime::db::EventRegistrationModel::ProtoPtrVec)

#endif // AUTOXTIME_DB_EVENTREGISTRATION
