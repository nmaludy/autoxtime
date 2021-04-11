#ifndef AUTOXTIME_DB_ORGANIZATIONMODEL
#define AUTOXTIME_DB_ORGANIZATIONMODEL

#include <autoxtime/db/db.h>
#include <autoxtime/db/BaseModelT.h>

namespace autoxtime { namespace proto { class Organization; } }

AUTOXTIME_DB_NAMESPACE_BEG

class OrganizationModel : public BaseModelT<autoxtime::proto::Organization, OrganizationModel>
{
  Q_OBJECT

 public:
  static const std::string TABLE;
  static const std::string PRIMARY_KEY;

  explicit OrganizationModel(QObject* pParent = nullptr);
  explicit OrganizationModel(std::shared_ptr<DbConnection> pConnection,
                             QObject* pParent = nullptr);

  virtual void emitSignal(Signal signal,
                          const std::vector<std::shared_ptr<autoxtime::proto::Organization> >& results);

 signals:
  // need the full namespace here so it matches the Q_DECLARE_METATYPE below
  void listResult(const std::vector<std::shared_ptr<autoxtime::proto::Organization> >& protoList);
  void createResult(const std::vector<std::shared_ptr<autoxtime::proto::Organization> >& protoList);
  void updateResult(const std::vector<std::shared_ptr<autoxtime::proto::Organization> >& protoList);
  void findResult(const std::vector<std::shared_ptr<autoxtime::proto::Organization> >& protoList);
};

AUTOXTIME_DB_NAMESPACE_END

// needed so we can use signals/slots with this type
Q_DECLARE_METATYPE(std::vector<std::shared_ptr<autoxtime::proto::Organization>>)

#endif // AUTOXTIME_DB_ORGANIZATION
