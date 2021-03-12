#ifndef AUTOXTIME_DB_DRIVERMODEL
#define AUTOXTIME_DB_DRIVERMODEL

#include <autoxtime/db/db.h>
#include <autoxtime/db/BaseModel.h>

namespace autoxtime::proto {
class Driver;
}

AUTOXTIME_DB_NAMESPACE_BEG

class DriverModel : public BaseModel
{
  Q_OBJECT;

 public:
  static const std::string TABLE;
  static const QString TABLE_Q;

  DriverModel(std::shared_ptr<DbConnection> pConnection = std::shared_ptr<DbConnection>());

  std::vector<std::shared_ptr<autoxtime::proto::Driver> > list();
  bool create(const autoxtime::proto::Driver& driver);
  bool update(const autoxtime::proto::Driver& driver);
  bool destroyById(int id);
  bool find(const autoxtime::proto::Driver& driver);
  bool findById(int id);

  virtual const std::string& table() const;
  virtual const QString&     tableQ() const;
};

AUTOXTIME_DB_NAMESPACE_END

#endif // AUTOXTIME_DB_DRIVER
