#include <autoxtime/db/CarClassModel.h>

#include <autoxtime/db/DbConnection.h>
#include <autoxtime/proto/car_class.pb.h>

#include <QtDebug>

AUTOXTIME_DB_NAMESPACE_BEG

const std::string CarClassModel::TABLE = "car_class";
const std::string CarClassModel::PRIMARY_KEY = "car_class_id";

CarClassModel::CarClassModel(QObject* pParent)
    : CarClassModel(std::shared_ptr<DbConnection>(), pParent)
{}

CarClassModel::CarClassModel(std::shared_ptr<DbConnection> pConnection,
                         QObject* pParent)
    : BaseModel(CarClassModel::TABLE,
                CarClassModel::PRIMARY_KEY,
                autoxtime::proto::CarClass::GetDescriptor(),
                autoxtime::proto::CarClass::GetReflection(),
                pConnection,
                pParent)
{}

std::vector<std::shared_ptr<autoxtime::proto::CarClass> > CarClassModel::list()
{
  return BaseModel::listT<autoxtime::proto::CarClass>();
}

std::vector<std::shared_ptr<autoxtime::proto::CarClass> > CarClassModel
::create(const autoxtime::proto::CarClass& carClass)
{
  return BaseModel::createT(carClass);
}

std::vector<std::shared_ptr<autoxtime::proto::CarClass> > CarClassModel
::update(const autoxtime::proto::CarClass& carClass)
{
  return BaseModel::updateT(carClass);
}

std::vector<std::shared_ptr<autoxtime::proto::CarClass> > CarClassModel
::find(const autoxtime::proto::CarClass& prototype)
{
  return BaseModel::findT<autoxtime::proto::CarClass>(prototype);
}

std::vector<std::shared_ptr<autoxtime::proto::CarClass> > CarClassModel
::findById(int id)
{
  return BaseModel::findByIdT<autoxtime::proto::CarClass>(id);
}

std::vector<std::shared_ptr<autoxtime::proto::CarClass> > CarClassModel
::createIfNotExists(const autoxtime::proto::CarClass& carClass)
{
  // creates carClass if doesn't exist, order of lookup is:
  // - name + subclass_name
  std::vector<std::shared_ptr<autoxtime::proto::CarClass> > results;

  // primary key
  if (carClass.has_car_class_id())
  {
    autoxtime::proto::CarClass proto;
    proto.set_car_class_id(carClass.car_class_id());
    results = find(proto);
    if (!results.empty())
    {
      return results;
    }
  }

  // name + subclass_name
  if (carClass.has_name())
  {
    autoxtime::proto::CarClass proto;
    proto.set_name(carClass.name());
    if (carClass.has_subclass_name())
    {
      proto.set_subclass_name(carClass.subclass_name());
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
    results = create(carClass);
  }
  return results;
}

AUTOXTIME_DB_NAMESPACE_END
