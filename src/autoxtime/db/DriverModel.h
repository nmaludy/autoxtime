#ifndef AUTOXTIME_DB_DRIVERMODEL
#define AUTOXTIME_DB_DRIVERMODEL

#include <autoxtime/db/db.h>
#include <autoxtime/db/BaseModel.h>

namespace autoxtime::proto { class Driver; }

AUTOXTIME_DB_NAMESPACE_BEG

class DriverModel : public BaseModel
{
  Q_OBJECT;

 public:
  static const std::string TABLE;
  static const std::string PRIMARY_KEY;

  DriverModel(QObject* pParent = nullptr);
  DriverModel(std::shared_ptr<DbConnection> pConnection,
              QObject* pParent = nullptr);

  std::vector<std::shared_ptr<autoxtime::proto::Driver> > list();
  std::vector<std::shared_ptr<autoxtime::proto::Driver> > create(const autoxtime::proto::Driver& driver);
  std::vector<std::shared_ptr<autoxtime::proto::Driver> > update(const autoxtime::proto::Driver& driver);

  std::vector<std::shared_ptr<autoxtime::proto::Driver> > find(const autoxtime::proto::Driver& prototype);
  std::vector<std::shared_ptr<autoxtime::proto::Driver> > findById(int id);

  // creates driver if doesn't exist, order of lookup is:
  // - scca_id
  // - msr_id
  // - first_name + last_name
  // - email
  // - phone_number
  std::vector<std::shared_ptr<autoxtime::proto::Driver> > createIfNotExists(const autoxtime::proto::Driver& driver);
};

AUTOXTIME_DB_NAMESPACE_END

#endif // AUTOXTIME_DB_DRIVER
