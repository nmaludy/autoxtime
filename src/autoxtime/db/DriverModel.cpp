#include <autoxtime/db/DriverModel.h>

#include <autoxtime/db/DbConnection.h>
#include <autoxtime/proto/driver.pb.h>

#include <QtDebug>

AUTOXTIME_DB_NAMESPACE_BEG

const std::string DriverModel::TABLE = "driver";
const QString DriverModel::TABLE_Q = QString::fromStdString(DriverModel::TABLE);

DriverModel::DriverModel(std::shared_ptr<DbConnection> pConnection)
    : BaseModel(autoxtime::proto::Driver::GetDescriptor(),
                autoxtime::proto::Driver::GetReflection(),
                pConnection)
{}

const std::string& DriverModel::table() const
{
  return DriverModel::TABLE;
}

const QString& DriverModel::tableQ() const
{
  return DriverModel::TABLE_Q;
}

std::vector<std::shared_ptr<autoxtime::proto::Driver> > DriverModel::list()
{
  return BaseModel::listT<autoxtime::proto::Driver>();
}

// bool create(const autoxtime::proto::Driver& driver);
// bool update(const autoxtime::proto::Driver& driver);
// bool destroy(const autoxtime::proto::Driver& driver);
// bool find(const autoxtime::proto::Driver& driver);
// bool findById(int id);

bool DriverModel::findBy(const autoxtime::proto::Driver& driver)
{
  DbConnection conn;
  QSqlDatabase db = conn.database();
  if (driver.has_first_name())
  {
    
  }
  return false;
}

AUTOXTIME_DB_NAMESPACE_END
