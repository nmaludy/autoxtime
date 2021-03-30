#ifndef AUTOXTIME_DB_CARMODEL
#define AUTOXTIME_DB_CARMODEL

#include <autoxtime/db/db.h>
#include <autoxtime/db/BaseModel.h>

namespace autoxtime::proto { class Car; }

AUTOXTIME_DB_NAMESPACE_BEG

class CarModel : public BaseModel
{
  Q_OBJECT;

 public:
  static const std::string TABLE;
  static const std::string PRIMARY_KEY;

  CarModel(QObject* pParent = nullptr);
  CarModel(std::shared_ptr<DbConnection> pConnection,
                QObject* pParent = nullptr);

  std::vector<std::shared_ptr<autoxtime::proto::Car> > list();
  std::vector<std::shared_ptr<autoxtime::proto::Car> > create(const autoxtime::proto::Car& car);
  std::vector<std::shared_ptr<autoxtime::proto::Car> > update(const autoxtime::proto::Car& car);

  std::vector<std::shared_ptr<autoxtime::proto::Car> > find(const autoxtime::proto::Car& prototype);
  std::vector<std::shared_ptr<autoxtime::proto::Car> > findById(int id);

  // creates Car if doesn't exist, order of lookup is:
  // - model + color + class_id + driver_id
  std::vector<std::shared_ptr<autoxtime::proto::Car> > createIfNotExists(const autoxtime::proto::Car& car);
};

AUTOXTIME_DB_NAMESPACE_END

#endif // AUTOXTIME_DB_CAR
