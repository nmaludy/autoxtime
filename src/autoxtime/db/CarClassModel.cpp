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
    : BaseModelT(CarClassModel::TABLE,
                 CarClassModel::PRIMARY_KEY,
                 autoxtime::proto::CarClass::GetDescriptor(),
                 autoxtime::proto::CarClass::GetReflection(),
                 pConnection,
                 pParent)
{}

void CarClassModel
::emitSignal(CarClassModel::Signal signal,
             const std::vector<std::shared_ptr<autoxtime::proto::CarClass> >& protoList)
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
