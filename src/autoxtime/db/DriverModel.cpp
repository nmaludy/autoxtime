#include <autoxtime/db/DriverModel.h>

#include <autoxtime/proto/driver.pb.h>

AUTOXTIME_DB_NAMESPACE_BEG

const std::string DriverModel::TABLE = "driver";
const std::string DriverModel::PRIMARY_KEY = "driver_id";

DriverModel::DriverModel(QObject* pParent)
    : DriverModel(std::shared_ptr<DbConnection>(), pParent)
{}

DriverModel::DriverModel(std::shared_ptr<DbConnection> pConnection,
                         QObject* pParent)
    : BaseModelT(DriverModel::TABLE,
                 DriverModel::PRIMARY_KEY,
                 autoxtime::proto::Driver::GetDescriptor(),
                 autoxtime::proto::Driver::GetReflection(),
                 pConnection,
                 pParent)
{}

void DriverModel
::emitSignal(DriverModel::Signal signal,
             const std::vector<std::shared_ptr<autoxtime::proto::Driver> >& protoList)
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

std::vector<std::shared_ptr<autoxtime::proto::Driver> > DriverModel
::createIfNotExists(const autoxtime::proto::Driver& driver)
{
  // creates driver if doesn't exist, order of lookup is:
  // - scca_id
  // - msr_id
  // - first_name + last_name
  // - email
  // - phone_number
  std::vector<std::shared_ptr<autoxtime::proto::Driver> > results;

  // primary key
  if (driver.has_driver_id())
  {
    autoxtime::proto::Driver proto;
    proto.set_driver_id(driver.driver_id());
    results = find(proto);
    if (!results.empty())
    {
      return results;
    }
  }

  // SCCA ID
  if (driver.has_scca_id())
  {
    autoxtime::proto::Driver proto;
    proto.set_scca_id(driver.scca_id());
    results = find(proto);
    if (!results.empty())
    {
      return results;
    }
  }

  // MSR ID
  if (driver.has_msr_id())
  {
    autoxtime::proto::Driver proto;
    proto.set_msr_id(driver.msr_id());
    results = find(proto);
    if (!results.empty())
    {
      return results;
    }
  }

  // first_name + last_name
  if (driver.has_first_name() && driver.has_last_name())
  {
    autoxtime::proto::Driver proto;
    proto.set_first_name(driver.first_name());
    proto.set_last_name(driver.last_name());
    results = find(proto);
    if (!results.empty())
    {
      return results;
    }
  }

  // email
  if (driver.has_email())
  {
    autoxtime::proto::Driver proto;
    proto.set_email(driver.email());
    results = find(proto);
    if (!results.empty())
    {
      return results;
    }
  }

  // phone_number
  if (driver.has_phone_number())
  {
    autoxtime::proto::Driver proto;
    proto.set_phone_number(driver.phone_number());
    results = find(proto);
    if (!results.empty())
    {
      return results;
    }
  }

  // create if we haven't found it yet
  if (results.empty())
  {
    results = create(driver);
  }
  return results;
}

AUTOXTIME_DB_NAMESPACE_END
