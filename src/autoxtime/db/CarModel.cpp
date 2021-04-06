#include <autoxtime/db/CarModel.h>

#include <autoxtime/db/DbConnection.h>
#include <autoxtime/proto/car.pb.h>

#include <QtDebug>

AUTOXTIME_DB_NAMESPACE_BEG

const std::string CarModel::TABLE = "car";
const std::string CarModel::PRIMARY_KEY = "car_id";

CarModel::CarModel(QObject* pParent)
    : CarModel(std::shared_ptr<DbConnection>(), pParent)
{}

CarModel::CarModel(std::shared_ptr<DbConnection> pConnection,
                         QObject* pParent)
    : BaseModel(CarModel::TABLE,
                CarModel::PRIMARY_KEY,
                autoxtime::proto::Car::GetDescriptor(),
                autoxtime::proto::Car::GetReflection(),
                pConnection,
                pParent)
{}

std::vector<std::shared_ptr<autoxtime::proto::Car> > CarModel::list()
{
  return BaseModel::listT<autoxtime::proto::Car>();
}

std::vector<std::shared_ptr<autoxtime::proto::Car> > CarModel
::create(const autoxtime::proto::Car& car)
{
  return BaseModel::createT(car);
}

std::vector<std::shared_ptr<autoxtime::proto::Car> > CarModel
::update(const autoxtime::proto::Car& car)
{
  return BaseModel::updateT(car);
}

std::vector<std::shared_ptr<autoxtime::proto::Car> > CarModel
::find(const autoxtime::proto::Car& prototype)
{
  return BaseModel::findT<autoxtime::proto::Car>(prototype);
}

std::vector<std::shared_ptr<autoxtime::proto::Car> > CarModel
::findById(int id)
{
  return BaseModel::findByIdT<autoxtime::proto::Car>(id);
}

std::vector<std::shared_ptr<autoxtime::proto::Car> > CarModel
::createIfNotExists(const autoxtime::proto::Car& car)
{
  // creates car if doesn't exist, order of lookup is:
  // - model + color + class_id + driver_id
  std::vector<std::shared_ptr<autoxtime::proto::Car> > results;

  // primary key
  if (car.has_car_id())
  {
    autoxtime::proto::Car proto;
    proto.set_car_id(car.car_id());
    results = find(proto);
    if (!results.empty())
    {
      return results;
    }
  }

  // model + class_id + driver_id + color (optional)
  if (car.has_model() &&
      car.has_car_class_id() &&
      car.has_driver_id())
  {
    autoxtime::proto::Car proto;
    proto.set_model(car.model());
    proto.set_car_class_id(car.car_class_id());
    proto.set_driver_id(car.driver_id());
    // color is optional
    if (car.has_color())
    {
      proto.set_color(car.color());
    }

    results = find(proto);
    if (!results.empty())
    {
      return results;
    }
  }

  // create if we haven't found it yet
  if (results.empty())
  {
    results = create(car);
  }
  return results;
}

AUTOXTIME_DB_NAMESPACE_END
