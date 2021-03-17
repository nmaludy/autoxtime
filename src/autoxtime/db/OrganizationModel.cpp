#include <autoxtime/db/OrganizationModel.h>

#include <autoxtime/db/DbConnection.h>
#include <autoxtime/proto/organization.pb.h>

#include <QtDebug>
#include <QtConcurrent/QtConcurrent>

AUTOXTIME_DB_NAMESPACE_BEG

const std::string OrganizationModel::TABLE = "organization";
const std::string OrganizationModel::PRIMARY_KEY = "organization_id";

OrganizationModel::OrganizationModel(QObject* pParent)
    : OrganizationModel(std::shared_ptr<DbConnection>(), pParent)
{}

OrganizationModel::OrganizationModel(std::shared_ptr<DbConnection> pConnection,
                                     QObject* pParent)
    : BaseModel(OrganizationModel::TABLE,
                OrganizationModel::PRIMARY_KEY,
                autoxtime::proto::Organization::GetDescriptor(),
                autoxtime::proto::Organization::GetReflection(),
                pConnection,
                pParent)
{
  qRegisterMetaType<OrganizationModel::ProtoPtrVec>();
}

OrganizationModel::ProtoPtrVec OrganizationModel::list()
{
  return BaseModel::listT<OrganizationModel::Proto>();
}

QFuture<OrganizationModel::ProtoPtrVec> OrganizationModel::listAsync()
{
  return QtConcurrent::run([=]() {
    // Code in this block will run in another thread

    // TODO, can we avoid createing new models for each query?
    // right now we are doing this for thread safety, probably for the better (honestly)
    std::unique_ptr<OrganizationModel> p_model = std::make_unique<OrganizationModel>();
    OrganizationModel::ProtoPtrVec results = p_model->list();
    emit listResult(results);
    return results;
  });
}

int OrganizationModel::create(const OrganizationModel::Proto& organization)
{
  return BaseModel::createT(organization);
}

int OrganizationModel::update(const OrganizationModel::Proto& organization)
{
  return BaseModel::updateT(organization);
}

OrganizationModel::ProtoPtrVec OrganizationModel::find(const OrganizationModel::Proto& prototype)
{
  return BaseModel::findT<OrganizationModel::Proto>(prototype);
}

OrganizationModel::ProtoPtrVec OrganizationModel::findById(int id)
{
  return BaseModel::findByIdT<OrganizationModel::Proto>(id);
}

AUTOXTIME_DB_NAMESPACE_END
