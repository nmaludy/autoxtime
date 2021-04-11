#include <autoxtime/db/OrganizationModel.h>

#include <autoxtime/db/DbConnection.h>
#include <autoxtime/proto/organization.pb.h>

#include <QtDebug>

AUTOXTIME_DB_NAMESPACE_BEG

const std::string OrganizationModel::TABLE = "organization";
const std::string OrganizationModel::PRIMARY_KEY = "organization_id";

OrganizationModel::OrganizationModel(QObject* pParent)
    : OrganizationModel(std::shared_ptr<DbConnection>(), pParent)
{}

OrganizationModel::OrganizationModel(std::shared_ptr<DbConnection> pConnection,
                                     QObject* pParent)
    : BaseModelT(OrganizationModel::TABLE,
                 OrganizationModel::PRIMARY_KEY,
                 autoxtime::proto::Organization::GetDescriptor(),
                 autoxtime::proto::Organization::GetReflection(),
                 pConnection,
                 pParent)
{
  qRegisterMetaType<OrganizationModel::ProtoPtrVec>();
}

void OrganizationModel
::emitSignal(OrganizationModel::Signal signal,
             const std::vector<std::shared_ptr<autoxtime::proto::Organization> >& protoList)
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
