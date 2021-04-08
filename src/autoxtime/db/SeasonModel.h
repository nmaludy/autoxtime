#ifndef AUTOXTIME_DB_SEASONMODEL
#define AUTOXTIME_DB_SEASONMODEL

#include <autoxtime/db/db.h>
#include <autoxtime/db/BaseModel.h>

namespace autoxtime { namespace proto { class Season; } }

AUTOXTIME_DB_NAMESPACE_BEG

class SeasonModel : public BaseModel
{
  Q_OBJECT

 public:
  static const std::string TABLE;
  static const std::string PRIMARY_KEY;

  typedef autoxtime::proto::Season Proto;
  typedef std::shared_ptr<Proto> ProtoPtr;
  typedef std::vector<ProtoPtr> ProtoPtrVec;

  explicit SeasonModel(QObject* pParent = nullptr);
  explicit SeasonModel(std::shared_ptr<DbConnection> pConnection,
                             QObject* pParent = nullptr);

  ProtoPtrVec list();
  QFuture<ProtoPtrVec> listAsync();

  ProtoPtrVec create(const Proto& season);
  ProtoPtrVec update(const Proto& season);

  ProtoPtrVec find(const Proto& prototype);
  ProtoPtrVec findById(std::int64_t id);

 signals:
  // need the full namespace here so it matches the Q_DECLARE_METATYPE below
  void listResult(const autoxtime::db::SeasonModel::ProtoPtrVec& protoList);
};

AUTOXTIME_DB_NAMESPACE_END

// needed so we can use signals/slots with this type
Q_DECLARE_METATYPE(autoxtime::db::SeasonModel::ProtoPtrVec)

#endif // AUTOXTIME_DB_SEASON
