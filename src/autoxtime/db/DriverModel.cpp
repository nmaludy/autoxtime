#include <autoxtime/db/DriverModel.h>

#include <autoxtime/db/DbConnection.h>
#include <autoxtime/proto/driver.pb.h>

#include <QtDebug>

AUTOXTIME_DB_NAMESPACE_BEG

const std::string DriverModel::TABLE = "driver";
const std::string DriverModel::PRIMARY_KEY = "driver_id";

DriverModel::DriverModel(QObject* pParent)
    : DriverModel(std::shared_ptr<DbConnection>(), pParent)
{}

DriverModel::DriverModel(std::shared_ptr<DbConnection> pConnection,
                         QObject* pParent)
    : BaseModel(DriverModel::TABLE,
                DriverModel::PRIMARY_KEY,
                autoxtime::proto::Driver::GetDescriptor(),
                autoxtime::proto::Driver::GetReflection(),
                pConnection,
                pParent)
{}

std::vector<std::shared_ptr<autoxtime::proto::Driver> > DriverModel::list()
{
  return BaseModel::listT<autoxtime::proto::Driver>();
}

std::vector<std::shared_ptr<autoxtime::proto::Driver> > DriverModel
::create(const autoxtime::proto::Driver& driver)
{
  return BaseModel::createT(driver);
}

std::vector<std::shared_ptr<autoxtime::proto::Driver> > DriverModel
::update(const autoxtime::proto::Driver& driver)
{
  return BaseModel::updateT(driver);
}

std::vector<std::shared_ptr<autoxtime::proto::Driver> > DriverModel
::find(const autoxtime::proto::Driver& prototype)
{
  return BaseModel::findT<autoxtime::proto::Driver>(prototype);
}

std::vector<std::shared_ptr<autoxtime::proto::Driver> > DriverModel
::findById(int id)
{
  return BaseModel::findByIdT<autoxtime::proto::Driver>(id);
}

AUTOXTIME_DB_NAMESPACE_END
