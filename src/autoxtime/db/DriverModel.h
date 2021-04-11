#ifndef AUTOXTIME_DB_DRIVERMODEL
#define AUTOXTIME_DB_DRIVERMODEL

#include <autoxtime/db/db.h>
#include <autoxtime/db/BaseModelT.h>

namespace autoxtime { namespace proto { class Driver; } }

AUTOXTIME_DB_NAMESPACE_BEG

class DriverModel : public BaseModelT<autoxtime::proto::Driver, DriverModel>
{
  Q_OBJECT

 public:
  static const std::string TABLE;
  static const std::string PRIMARY_KEY;

  DriverModel(QObject* pParent = nullptr);
  DriverModel(std::shared_ptr<DbConnection> pConnection,
              QObject* pParent = nullptr);

  // creates driver if doesn't exist, order of lookup is:
  // - scca_id
  // - msr_id
  // - first_name + last_name
  // - email
  // - phone_number
  std::vector<std::shared_ptr<autoxtime::proto::Driver> > createIfNotExists(const autoxtime::proto::Driver& driver);

  virtual void emitSignal(Signal signal,
                          const std::vector<std::shared_ptr<autoxtime::proto::Driver> >& results);

 signals:
  // need the full namespace here so it matches the Q_DECLARE_METATYPE below
  void listResult(const std::vector<std::shared_ptr<autoxtime::proto::Driver> >& protoList);
  void createResult(const std::vector<std::shared_ptr<autoxtime::proto::Driver> >& protoList);
  void updateResult(const std::vector<std::shared_ptr<autoxtime::proto::Driver> >& protoList);
  void findResult(const std::vector<std::shared_ptr<autoxtime::proto::Driver> >& protoList);
};

AUTOXTIME_DB_NAMESPACE_END

// needed so we can use signals/slots with this type
Q_DECLARE_METATYPE(std::vector<std::shared_ptr<autoxtime::proto::Driver>>)

#endif // AUTOXTIME_DB_DRIVER
