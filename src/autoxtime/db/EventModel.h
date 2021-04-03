#ifndef AUTOXTIME_DB_EVENTMODEL
#define AUTOXTIME_DB_EVENTMODEL

#include <autoxtime/db/db.h>
#include <autoxtime/db/BaseModel.h>

namespace autoxtime::proto {
class Event;
}

AUTOXTIME_DB_NAMESPACE_BEG

class EventModel : public BaseModel
{
  Q_OBJECT;

 public:
  static const std::string TABLE;
  static const std::string PRIMARY_KEY;

  typedef autoxtime::proto::Event Proto;
  typedef std::shared_ptr<Proto> ProtoPtr;
  typedef std::vector<ProtoPtr> ProtoPtrVec;

  explicit EventModel(QObject* pParent = nullptr);
  explicit EventModel(std::shared_ptr<DbConnection> pConnection,
                      QObject* pParent = nullptr);

  ProtoPtrVec list();
  QFuture<ProtoPtrVec> listAsync();

  ProtoPtrVec create(const Proto& event);
  ProtoPtrVec update(const Proto& event);

  ProtoPtrVec find(const Proto& prototype);
  ProtoPtrVec findById(int id);

 signals:
  // need the full namespace here so it matches the Q_DECLARE_METATYPE below
  void listResult(const autoxtime::db::EventModel::ProtoPtrVec& protoList);
};

AUTOXTIME_DB_NAMESPACE_END

// needed so we can use signals/slots with this type
Q_DECLARE_METATYPE(autoxtime::db::EventModel::ProtoPtrVec);

#endif // AUTOXTIME_DB_EVENT
