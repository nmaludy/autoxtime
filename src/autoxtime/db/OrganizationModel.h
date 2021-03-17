#ifndef AUTOXTIME_DB_ORGANIZATIONMODEL
#define AUTOXTIME_DB_ORGANIZATIONMODEL

#include <autoxtime/db/db.h>
#include <autoxtime/db/BaseModel.h>

namespace autoxtime::proto {
class Organization;
}

AUTOXTIME_DB_NAMESPACE_BEG

class OrganizationModel : public BaseModel
{
  Q_OBJECT;

 public:
  static const std::string TABLE;
  static const std::string PRIMARY_KEY;

  typedef autoxtime::proto::Organization Proto;
  typedef std::shared_ptr<Proto> ProtoPtr;
  typedef std::vector<ProtoPtr> ProtoPtrVec;

  explicit OrganizationModel(QObject* pParent = nullptr);
  OrganizationModel(std::shared_ptr<DbConnection> pConnection,
                    QObject* pParent = nullptr);

  ProtoPtrVec list();
  QFuture<ProtoPtrVec> listAsync();

  int create(const Proto& organization);
  int update(const Proto& organization);

  ProtoPtrVec find(const Proto& prototype);
  ProtoPtrVec findById(int id);

 signals:
  // need the full namespace here so it matches the Q_DECLARE_METATYPE below
  void listResult(const autoxtime::db::OrganizationModel::ProtoPtrVec& protoList);
};

AUTOXTIME_DB_NAMESPACE_END

// needed so we can use signals/slots with this type
Q_DECLARE_METATYPE(autoxtime::db::OrganizationModel::ProtoPtrVec);

#endif // AUTOXTIME_DB_ORGANIZATION
