#include <autoxtime/ui/RunTableModel.h>

#include <autoxtime/db/CarModel.h>
#include <autoxtime/db/CarClassModel.h>
#include <autoxtime/db/DriverModel.h>
#include <autoxtime/db/EventModel.h>
#include <autoxtime/db/EventRegistrationModel.h>
#include <autoxtime/db/RunModel.h>
#include <autoxtime/log/Log.h>
#include <autoxtime/proto/car.pb.h>
#include <autoxtime/proto/car_class.pb.h>
#include <autoxtime/proto/driver.pb.h>
#include <autoxtime/proto/event.pb.h>
#include <autoxtime/proto/run.pb.h>
#include <autoxtime/util/ColorUtil.h>

#include <QApplication>

using autoxtime::db::CarModel;
using autoxtime::db::CarClassModel;
using autoxtime::db::DriverModel;
using autoxtime::db::EventModel;
using autoxtime::db::EventRegistrationModel;
using autoxtime::db::RunModel;

AUTOXTIME_UI_NAMESPACE_BEG

const std::unordered_map<RunTableModel::TableColumn, QString> RunTableModel::COLUMN_HEADER_MAP = {
  { TABLE_COLUMN_RUN_ID,  "Run ID" },
  { TABLE_COLUMN_PREVIOUS_RUN_ID,  "Previous Run ID" },
  { TABLE_COLUMN_RUN_NUMBER,  "Run" },
  { TABLE_COLUMN_CLASS, "Class" },
  { TABLE_COLUMN_CAR_NUM, "Number" },
  { TABLE_COLUMN_CAR_MODEL, "Car Model" },
  { TABLE_COLUMN_DRIVER_NAME, "Driver" },
  { TABLE_COLUMN_TIME_SECTOR_1, "S1" },
  { TABLE_COLUMN_TIME_SECTOR_2, "S2" },
  { TABLE_COLUMN_TIME_TOTAL, "Time" },
  { TABLE_COLUMN_DNF, "DNF" },
  { TABLE_COLUMN_PENALTIES, "Penalties" },
};

RunTableModel::RunTableModel(QWidget* pParent)
    : QAbstractTableModel(pParent),
      mpCarClassModel(new autoxtime::db::CarClassModel(this)),
      mpCarModel(new autoxtime::db::CarModel(this)),
      mpDriverModel(new autoxtime::db::DriverModel(this)),
      mpRunModel(new autoxtime::db::RunModel(this)),
      mCarsPopulated(false),
      mCarClassesPopulated(false),
      mDriversPopulated(false),
      mRunsPopulated(false),
      mEventId(-1)
{
  // model signals
  connect(mpCarModel, &autoxtime::db::CarModel::findResult,
          this,       &RunTableModel::setCars);
  connect(mpCarModel, &autoxtime::db::CarModel::notification,
          this,       &RunTableModel::carNotification);

  connect(mpCarClassModel, &autoxtime::db::CarClassModel::findResult,
          this,            &RunTableModel::setCarClasses);
  connect(mpCarClassModel, &autoxtime::db::CarClassModel::notification,
          this,            &RunTableModel::carClassNotification);

  connect(mpDriverModel, &autoxtime::db::DriverModel::findResult,
          this,          &RunTableModel::setDrivers);
  connect(mpDriverModel, &autoxtime::db::DriverModel::notification,
          this,          &RunTableModel::driverNotification);

  connect(mpRunModel, &autoxtime::db::RunModel::findResult,
          this,       &RunTableModel::setRuns);
  connect(mpRunModel, &autoxtime::db::RunModel::notification,
          this,       &RunTableModel::runNotification);

  // notifications
  mpCarModel->subscribeToNotifications();
  mpCarClassModel->subscribeToNotifications();
  mpDriverModel->subscribeToNotifications();
  mpRunModel->subscribeToNotifications();
}

////////////////////////////////////////////////////////////////////////////////
// BEGIN Qt Model Functions

// When subclassing QAbstractTableModel, you must implement rowCount(), columnCount(), and data().
// Well behaved models will also implement headerData().
// Editable models need to implement setData(), and implement flags() to return a value containing Qt::ItemIsEditable.

int RunTableModel::columnCount(const QModelIndex& parent) const
{
  return COLUMN_HEADER_MAP.size();
}

int RunTableModel::rowCount(const QModelIndex& parent) const
{
  return mDataItems.size();
}

std::shared_ptr<RunTableModel::DataItem> RunTableModel
::itemFromIndex(const QModelIndex& index) const
{
  if (index.row() < 0 || index.row() >= rowCount() ||
      index.column() < 0 || index.column() >= columnCount())
  {
    return std::shared_ptr<DataItem>();
  }
  return mDataItems[index.row()];
}

QVariant RunTableModel::data(const QModelIndex& index, int role) const
{
  QVariant data;
  const std::shared_ptr<DataItem> p_item = itemFromIndex(index);
  if (p_item == nullptr)
  {
    return data;
  }

  // When an editor opens it asks for data in the Qt::EditRole
  // We want to return the same data the user was looking at when they started editing
  // Thus, this should be the same as the Qt::DisplayRole.
  role = (role == Qt::EditRole) ? Qt::DisplayRole : role;

  switch (static_cast<TableColumn>(index.column()))
  {
    case TABLE_COLUMN_RUN_ID:
      if (p_item->mpRun != nullptr && role == Qt::DisplayRole)
      {
        data = QVariant::fromValue(p_item->mpRun->run_id());
      }
      break;
    case TABLE_COLUMN_PREVIOUS_RUN_ID:
      if (p_item->mpRun != nullptr && role == Qt::DisplayRole)
      {
        if (p_item->mpRun->has_previous_run_id())
        {
          data = QVariant::fromValue(p_item->mpRun->previous_run_id());
        }
        else
        {
          // return invalid QVariant if previous run wasn't set
          data = QVariant();
        }
      }
      break;
    case TABLE_COLUMN_RUN_NUMBER:
      // TODO
      break;
    case TABLE_COLUMN_CLASS:
      if (p_item->mpCarClass != nullptr && role == Qt::DisplayRole)
      {
        std::string name = p_item->mpCarClass->name();
        if (p_item->mpCarClass->has_subclass_name() &&
            !p_item->mpCarClass->subclass_name().empty())
        {
          name += "-" + p_item->mpCarClass->subclass_name();
        }
        data = QVariant(QString::fromStdString(name));
      }
      break;
    case TABLE_COLUMN_CAR_NUM:
      if (p_item->mpCar != nullptr && role == Qt::DisplayRole)
      {
        // need this to be a regular into here, because if we return a longlong (int64)
        // the editor doesn't render properly
        data = QVariant::fromValue<int>(p_item->mpCar->car_number());
      }
      break;
    case TABLE_COLUMN_DRIVER_NAME:
      if (p_item->mpDriver != nullptr && role == Qt::DisplayRole)
      {
        data = QVariant(QString::fromStdString(
            p_item->mpDriver->first_name() + " " +
            p_item->mpDriver->last_name()));
      }
      break;
    case TABLE_COLUMN_CAR_MODEL:
      if (p_item->mpCar == nullptr) { break; }
      switch (role)
      {
        case Qt::DisplayRole:
          data = QVariant(QString::fromStdString(p_item->mpCar->model()));
          break;
        case Qt::DecorationRole:
          if (p_item->mCarColor.isValid())
          {
            data = p_item->mCarColor;
            // TODO if white, need to paint a black border or something
          }
          else
          {
            // TODO use a custom ? icon
            data = QApplication::style()->standardIcon(QStyle::SP_MessageBoxQuestion);
          }
          break;
        default:
          break;
      }
      break;
    case TABLE_COLUMN_TIME_SECTOR_1:
      if (p_item->mpRun != nullptr && role == Qt::DisplayRole)
      {
        data = QVariant::fromValue(p_item->mpRun->sector1_time());
      }
      break;
    case TABLE_COLUMN_TIME_SECTOR_2:
      if (p_item->mpRun != nullptr && role == Qt::DisplayRole)
      {
        data = QVariant::fromValue(p_item->mpRun->sector2_time());
      }
      break;
    case TABLE_COLUMN_TIME_TOTAL:
      if (p_item->mpRun != nullptr && role == Qt::DisplayRole && p_item->mpRun->has_end_time())
      {
        data = QVariant::fromValue(
            p_item->mpRun->end_time() - p_item->mpRun->start_time());
      }
      break;
    case TABLE_COLUMN_DNF:
      if (p_item->mpRun == nullptr) { return QVariant(); }
      switch (role)
      {
        case Qt::DisplayRole:
          data = QVariant::fromValue(p_item->mpRun->dnf());
          break;
        case Qt::CheckStateRole:
          data = QVariant::fromValue(p_item->mpRun->dnf() ?
                                     Qt::Checked :
                                     Qt::Unchecked);
        default:
          break;
      }
      break;
    case TABLE_COLUMN_PENALTIES:
      if (p_item->mpRun != nullptr && role == Qt::DisplayRole)
      {
        data = QVariant::fromValue(p_item->mpRun->penalties());
      }
      break;
  }
  return data;
}

QVariant RunTableModel::headerData(int section,
                                   Qt::Orientation orientation,
                                   int role) const
{
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
  {
    return QVariant(COLUMN_HEADER_MAP.at(static_cast<TableColumn>(section)));
  }
  return QVariant();
}

bool RunTableModel::setData(const QModelIndex& index,
                            const QVariant& value,
                            int role)
{
  bool b_updated = false;
  const std::shared_ptr<DataItem> p_item = itemFromIndex(index);
  if (p_item == nullptr || role != Qt::EditRole)
  {
    return b_updated;
  }

  // only update if the data changed
  QVariant old = data(index, role);
  if (value == old)
  {
    return b_updated;
  }

  switch (static_cast<TableColumn>(index.column()))
  {
    case TABLE_COLUMN_RUN_ID: break;
    case TABLE_COLUMN_PREVIOUS_RUN_ID: break;
    case TABLE_COLUMN_RUN_NUMBER: break;
    case TABLE_COLUMN_CLASS:
      // TODO
      break;
    case TABLE_COLUMN_CAR_NUM:
      // TODO
      break;
    case TABLE_COLUMN_DRIVER_NAME: break;
    case TABLE_COLUMN_CAR_MODEL: break;
    case TABLE_COLUMN_TIME_SECTOR_1: break;
    case TABLE_COLUMN_TIME_SECTOR_2: break;
    case TABLE_COLUMN_TIME_TOTAL: break;
    case TABLE_COLUMN_DNF:
      // TODO
      break;
    case TABLE_COLUMN_PENALTIES:
      // TODO
      break;
  }

  if (b_updated)
  {
    QVector<int> roles;
    roles << role;
    emit dataChanged(index, index, roles);
  }
  return b_updated;
}

Qt::ItemFlags RunTableModel::flags(const QModelIndex &index) const
{
  Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  switch (static_cast<TableColumn>(index.column()))
  {
    case TABLE_COLUMN_RUN_ID: break;
    case TABLE_COLUMN_PREVIOUS_RUN_ID: break;
    case TABLE_COLUMN_RUN_NUMBER: break;
    case TABLE_COLUMN_CLASS:
      flags |= Qt::ItemIsEditable;
      break;
    case TABLE_COLUMN_CAR_NUM:
      flags |= Qt::ItemIsEditable;
      break;
    case TABLE_COLUMN_DRIVER_NAME: break;
    case TABLE_COLUMN_CAR_MODEL: break;
    case TABLE_COLUMN_TIME_SECTOR_1: break;
    case TABLE_COLUMN_TIME_SECTOR_2: break;
    case TABLE_COLUMN_TIME_TOTAL: break;
    case TABLE_COLUMN_DNF:
      flags |= Qt::ItemIsEditable;
      break;
    case TABLE_COLUMN_PENALTIES:
      flags |= Qt::ItemIsEditable;
      break;
  }
  return flags;
}

// END Qt Model Functions
////////////////////////////////////////////////////////////////////////////////

void RunTableModel::setEventId(std::int64_t eventId)
{
  mEventId = eventId;

  // clear current table, only do this when event changes
  reset();

  // find all event runs, async SQL query
  // autoxtime::proto::Run run;
  // run.set_event_id(mEventId);
  // mpRunModel->findAsync(run);

  // find all drivers registered for this event
  {
    std::string bind_event_id = ":event_id";
    std::unordered_map<QString, QVariant> bindings;
    bindings[bind_event_id.data()] = QVariant::fromValue(mEventId);
    std::string custom =
        " ORDER BY " + CarModel::PRIMARY_KEY;
    mpRunModel->findCustomAsync(QString::fromStdString(custom), bindings);
  }
}

void RunTableModel::reset()
{
  beginResetModel();

  // clear our underlying state
  mCars.clear();
  mCarClasses.clear();
  mDrivers.clear();
  mRuns.clear();
  mDataItems.clear();

  mCarsPopulated = false;
  mCarClassesPopulated = false;
  mDriversPopulated = false;
  mRunsPopulated = false;

  endResetModel();
}

void RunTableModel
::setRuns(const std::vector<std::shared_ptr<autoxtime::proto::Run>>& runs)
{
  beginResetModel();

  AXT_DEBUG << "Found " << runs.size() << " runs for event id: "
            << mEventId;
  for (const std::shared_ptr<autoxtime::proto::Run>& p_run : runs)
  {
    updateRun(p_run);
  }

  // TODO add all of the event IDs, driver ids, etc so we can match them later
  std::string bind_event_id = ":event_id";
  std::unordered_map<QString, QVariant> bindings;
  bindings[bind_event_id.data()] = QVariant::fromValue(mEventId);

  // use EventRegistration below because we need to keep all of the cars/drivers/classes
  // that are registered for this event in memory, that way when a new run comes in
  // we already have the driver/car/class ready to go and don't have to go out and query
  // for it

  // find all drivers registered for this event
  {
    std::string custom =
        "JOIN " + EventRegistrationModel::TABLE
        + " ON " + DriverModel::TABLE + "." + DriverModel::PRIMARY_KEY
        + " = " + EventRegistrationModel::TABLE + "." + DriverModel::PRIMARY_KEY
        + " WHERE " + EventRegistrationModel::TABLE + "." + EventModel::PRIMARY_KEY
        + " = " + bind_event_id;
    mpDriverModel->findCustomAsync(QString::fromStdString(custom), bindings);
  }

  // find all cars registered for this event
  {
    std::string custom =
        "JOIN " + EventRegistrationModel::TABLE
        + " ON " + CarModel::TABLE + "." + CarModel::PRIMARY_KEY
        + " = " + EventRegistrationModel::TABLE + "." + CarModel::PRIMARY_KEY
        + " WHERE " + EventRegistrationModel::TABLE + "." + EventModel::PRIMARY_KEY
        + " = " + bind_event_id;
    mpCarModel->findCustomAsync(QString::fromStdString(custom), bindings);
  }

  // find all car classes in this event
  {
    std::string custom =
        "WHERE " + CarClassModel::TABLE + "." + CarClassModel::PRIMARY_KEY + " IN ( "
        + " SELECT DISTINCT " + CarModel::TABLE + "." + CarClassModel::PRIMARY_KEY
        + " FROM " + EventRegistrationModel::TABLE
        + " JOIN " + CarModel::TABLE
        + " ON " + CarModel::TABLE + "." + CarModel::PRIMARY_KEY
        + " = " + EventRegistrationModel::TABLE + "." + CarModel::PRIMARY_KEY
        + " WHERE " + EventRegistrationModel::TABLE + "." + EventModel::PRIMARY_KEY
        + " = " + bind_event_id + " )";
    mpCarClassModel->findCustomAsync(QString::fromStdString(custom), bindings);
  }
  mRunsPopulated = true;
}

bool RunTableModel
::updateRun(const std::shared_ptr<autoxtime::proto::Run>& pRun)
{
  // get current event ID and use it for binding in SQL queries
  if (pRun->event_id() != mEventId)
  {
    return false;
  }

  std::int64_t id = pRun->run_id();
  std::unordered_map<std::int64_t, std::shared_ptr<DataItem>>::iterator iter =
      mRuns.find(id);

  std::shared_ptr<DataItem> p_item;
  if (iter != mRuns.end())
  {
    AXT_DEBUG << "run id exists in the map: " << id;
    // mapping from ID -> item already exists
    p_item = iter->second;

    // if previous_run_id changes? need to move stuff around
    bool b_move_run = (p_item->mpRun->has_previous_run_id() != pRun->has_previous_run_id() ||
                       p_item->mpRun->previous_run_id() != pRun->previous_run_id());
    
    // update our event run point to contain the new data
    p_item->mpRun = pRun;

    if (b_move_run)
    {
      // remove run
      std::vector<std::shared_ptr<DataItem>>::iterator di_iter =
          std::find(mDataItems.begin(), mDataItems.end(), p_item);
      int row_idx = di_iter - mDataItems.begin();
      AXT_DEBUG << "run id [" << id << "] needs be moved, erasing row_idx=" << row_idx;
      beginRemoveRows(QModelIndex(), row_idx, row_idx);
      mDataItems.erase(di_iter);
      endRemoveRows();
      // insert it back into the right place, or add it to pending
      insertOrPendingRun(p_item);
      beginResetModel();
      endResetModel();
    }
  }
  else
  {
    AXT_DEBUG << "run id doesn't exists in the map: " << id;
    p_item = insertRun(pRun);

    // add mapping for this item
    mRuns[id] = p_item;
  }

  // map related IDs to this item
  std::unordered_map<std::int64_t, std::shared_ptr<DataItem>>::iterator driver_iter =
      mDrivers.find(pRun->driver_id());
  if (driver_iter != mDrivers.end())
  {
    AXT_DEBUG << "Run using existing driver id: " << pRun->driver_id();
    p_item->mpDriver = driver_iter->second->mpDriver;
  }
  else
  {
    AXT_DEBUG << "Run adding driver id: " << pRun->driver_id();
  }
  mDrivers[pRun->driver_id()] = p_item;

  std::unordered_map<std::int64_t, std::shared_ptr<DataItem>>::iterator car_iter =
      mCars.find(pRun->car_id());
  if (car_iter != mCars.end())
  {
    AXT_DEBUG << "Run using existing car id: " << pRun->car_id();
    p_item->mpCar = car_iter->second->mpCar;
    p_item->mCarColor = car_iter->second->mCarColor;
  }
  else
  {
    AXT_DEBUG << "Run adding car id: " << pRun->car_id();
  }
  mCars[pRun->car_id()] = p_item;

  // trigger data changed for the final update
  itemDataChanged(p_item);
  return true;
}

std::shared_ptr<RunTableModel::DataItem> RunTableModel
::insertRun(const std::shared_ptr<autoxtime::proto::Run>& pRun)
{
  std::shared_ptr<DataItem> p_item = std::make_shared<DataItem>();
  p_item->mpRun = pRun;
  insertOrPendingRun(p_item);
  return p_item;
}
 
void RunTableModel::insertOrPendingRun(std::shared_ptr<DataItem> pItem)
{
  std::vector<std::shared_ptr<DataItem>>::iterator insert_iter = mDataItems.end();
  std::vector<std::shared_ptr<DataItem>>::iterator new_iter = mDataItems.end();

  // is this the first run?
  int64_t run_id = pItem->mpRun->run_id();
  if (!pItem->mpRun->has_previous_run_id())
  {
    // insert at front
    AXT_DEBUG << "insertOrPendingRun - found first run: " << run_id;
    insert_iter = mDataItems.begin();
    beginInsertRows(QModelIndex(), 0, 0);
    new_iter = mDataItems.insert(insert_iter, pItem); // insert before insert_iter
    endInsertRows();
  }
  else
  {
    // not first run, find where it is supposed to go?
    // try to see if there is a run that matches our previous_run_id
    int64_t previous_run_id = pItem->mpRun->previous_run_id();
    insert_iter = std::find_if(mDataItems.begin(), mDataItems.end(),
                               [&] (const std::shared_ptr<DataItem>& pi)
                               {
                                 int64_t run_id = pi->mpRun->run_id();
                                 bool is_prev = run_id == previous_run_id;
                                 return is_prev;
                               }); 
    // did we find the previous run id?
    if (insert_iter != mDataItems.end())
    {
      ++insert_iter; // increment so the previous run is immediately before our insert
      int row_idx = insert_iter - mDataItems.begin();  // compute the new item's index
      AXT_DEBUG << "insertOrPendingRun - inserting run id [" << run_id << "] at row_idx=" << row_idx;
      beginInsertRows(QModelIndex(), row_idx, row_idx);
      new_iter = mDataItems.insert(insert_iter, pItem); // insert before insert_iter
      endInsertRows();
    }
    else
    {
      // add to pending since we couldn't find previous run id
      AXT_DEBUG << "insertOrPendingRun - assigning run id [" << run_id << "] to pending runs";
      if (mDataItemsPending.find(previous_run_id) == mDataItemsPending.end())
      {
        mDataItemsPending[previous_run_id] = pItem;
      }
      else
      {
        AXT_ERROR << "insertOrPendingRun - found multiple runs that have the same previous_run_id... exising run_id=" << mDataItemsPending[previous_run_id]->mpRun->run_id()
                  << " new run_id=" << run_id;
      }
    }
  }

  // if we inserted a new item
  if (new_iter != mDataItems.end())
  {
    std::unordered_map<std::int64_t, std::shared_ptr<DataItem>>::iterator pending_iter =
        mDataItemsPending.end();
    // loop finding items that have this item as their previous run id
    while ((pending_iter = mDataItemsPending.find(run_id)) != mDataItemsPending.end())
    {
      ++new_iter; // increment the previous run so we insert after it
      int row_idx = new_iter - mDataItems.begin();  // compute the new item's index
      // use this run's ID as lookup for next loop
      run_id = pending_iter->second->mpRun->run_id();
      AXT_DEBUG << "insertOrPendingRun - removing pending run id [" << run_id << "] and inserting at row_idx=" << row_idx;
      beginInsertRows(QModelIndex(), row_idx, row_idx);
      // transfer pending -> data items
      new_iter = mDataItems.insert(new_iter, pending_iter->second);
      mDataItemsPending.erase(pending_iter); // remove from pending
      endInsertRows();
    }
  }
}

void RunTableModel
::setDrivers(const std::vector<std::shared_ptr<autoxtime::proto::Driver>>& drivers)
{
  AXT_DEBUG << "Found " << drivers.size() << " drivers for event id: "
            << mEventId;
  for (const std::shared_ptr<autoxtime::proto::Driver>& p_driver : drivers)
  {
    updateDriver(p_driver);
  }
  mDriversPopulated = true;
}

bool RunTableModel
::updateDriver(const std::shared_ptr<autoxtime::proto::Driver>& pDriver)
{
  std::int64_t id = pDriver->driver_id();
  std::unordered_map<std::int64_t, std::shared_ptr<DataItem>>::iterator iter =
      mDrivers.find(id);
  if (iter != mDrivers.end())
  {
    std::shared_ptr<DataItem>& p_item = iter->second;
    p_item->mpDriver = pDriver;
    itemDataChanged(p_item);
    return true;
  }
  else
  {
    // add it to our database of known drivers
    std::shared_ptr<DataItem> p_item = std::make_shared<DataItem>();
    p_item->mpDriver = pDriver;
    mDrivers[id] = p_item;
  }
  return false;
}

void RunTableModel
::setCars(const std::vector<std::shared_ptr<autoxtime::proto::Car>>& cars)
{
  AXT_DEBUG << "Found " << cars.size() << " cars for event id: "
            << mEventId;

  for (const std::shared_ptr<autoxtime::proto::Car>& p_car : cars)
  {
    updateCar(p_car);
  }

  // update car classes in case they came back before the cars did
  updateCarClasses();
  mCarsPopulated = true;
}

bool RunTableModel
::updateCar(const std::shared_ptr<autoxtime::proto::Car>& pCar)
{
  // do we know about this yet or not?
  std::int64_t id = pCar->car_id();
  std::unordered_map<std::int64_t, std::shared_ptr<DataItem>>::iterator iter =
      mCars.find(id);
  if (iter != mCars.end())
  {
    std::shared_ptr<DataItem>& p_item = iter->second;
    p_item->mpCar = pCar;
    p_item->mCarColor = autoxtime::util::ColorUtil::nameToColor(pCar->color());
    itemDataChanged(p_item);
    return true;
  }
  else
  {
    // add it to our database of known drivers
    std::shared_ptr<DataItem> p_item = std::make_shared<DataItem>();
    p_item->mpCar = pCar;
    p_item->mCarColor = autoxtime::util::ColorUtil::nameToColor(pCar->color());
    mCars[id] = p_item;
  }
  return false;
}

void RunTableModel
::setCarClasses(const std::vector<std::shared_ptr<autoxtime::proto::CarClass>>& carClasses)
{
  AXT_DEBUG << "Found " << carClasses.size() << " cars classes for event id: "
            << mEventId;
  for (const std::shared_ptr<autoxtime::proto::CarClass>& p_cc : carClasses)
  {
    // note: don't call updateCarClass here because that function expects that we
    //       know about all of the car classes and also calls update() which is an
    //       expensive operation every time. that function is designed for updates from
    //       database, not model loading
    mCarClasses[p_cc->car_class_id()] = p_cc;
  }

  // update car classes in case they came back after the cars did
  updateCarClasses();
  mCarClassesPopulated = true;
}

void RunTableModel::updateCarClasses()
{
  AXT_DEBUG << "updateCarClasses - begin";
  if (mCars.empty() || mCarClasses.empty())
  {
    AXT_DEBUG << "updateCarClasses() - cars or classes are empty";
    return;
  }
  AXT_DEBUG << "updateCarClasses() - cars size = " << mCars.size()
            << " car classes size = " << mCarClasses.size();

  for (std::pair<const std::int64_t, std::shared_ptr<DataItem>>& id_item : mCars)
  {
    std::shared_ptr<DataItem>& p_item = id_item.second;
    if (p_item->mpCar == nullptr)
    {
      AXT_DEBUG << "upcateCarClasses() - car is null, can't get class";
      continue;
    }

    // lookup the car class by ID to get its name
    std::int64_t car_class_id = p_item->mpCar->car_class_id();
    std::unordered_map<std::int64_t, std::shared_ptr<autoxtime::proto::CarClass>>::iterator iter =
        mCarClasses.find(car_class_id);
    if (iter == mCarClasses.end())
    {
      AXT_DEBUG << "updateCarClasses() - couldn't find car class with id: " << car_class_id;
      continue;
    }

    std::shared_ptr<autoxtime::proto::CarClass>& p_cc = iter->second;
    p_item->mpCarClass = p_cc;
    itemDataChanged(p_item);
  }
  AXT_DEBUG << "updateCarClasses - done";
}

bool RunTableModel
::updateCarClass(const std::shared_ptr<autoxtime::proto::CarClass>& pCarClass)
{
  // do we know about this yet or not?
  std::int64_t car_class_id = pCarClass->car_class_id();
  std::unordered_map<std::int64_t, std::shared_ptr<autoxtime::proto::CarClass>>::iterator iter =
      mCarClasses.find(car_class_id);
  if (iter != mCarClasses.end())
  {
    std::shared_ptr<autoxtime::proto::CarClass>& p_cc = iter->second;
    *p_cc = *pCarClass;
    updateCarClasses();
    return true;
  }
  else
  {
    mCarClasses[car_class_id] = pCarClass;
  }
  return false;
}


void RunTableModel::carNotification(const std::shared_ptr<google::protobuf::Message>& pMessage,
                                    const QDateTime& timestamp,
                                    const QString& operation)
{
  AXT_DEBUG << "RunTableModel - car notification";
  std::shared_ptr<autoxtime::proto::Car> p_car = CarModel::messageToT(pMessage);
  updateCar(p_car);
}

void RunTableModel::carClassNotification(const std::shared_ptr<google::protobuf::Message>& pMessage,
                                         const QDateTime& timestamp,
                                         const QString& operation)
{
  AXT_DEBUG << "RunTableModel - car class notification";
  std::shared_ptr<autoxtime::proto::CarClass> p_car_class = CarClassModel::messageToT(pMessage);
  updateCarClass(p_car_class);
}

void RunTableModel::driverNotification(const std::shared_ptr<google::protobuf::Message>& pMessage,
                                       const QDateTime& timestamp,
                                       const QString& operation)
{
  AXT_DEBUG << "RunTableModel - driver notification " << operation << "\n"
            << pMessage->DebugString();
  std::shared_ptr<autoxtime::proto::Driver> p_driver = DriverModel::messageToT(pMessage);
  updateDriver(p_driver);
}

void RunTableModel::runNotification(const std::shared_ptr<google::protobuf::Message>& pMessage,
                                    const QDateTime& timestamp,
                                    const QString& operation)
{
  AXT_DEBUG << "RunTableModel - event run notification";
  std::shared_ptr<autoxtime::proto::Run> p_run = RunModel::messageToT(pMessage);
  updateRun(p_run);
}

void RunTableModel::itemDataChanged(const std::shared_ptr<DataItem>& pItem)
{
  std::vector<std::shared_ptr<DataItem>>::const_iterator iter =
      std::find(mDataItems.begin(), mDataItems.end(), pItem);
  if (iter != mDataItems.end())
  {
    int row = iter - mDataItems.begin();
    QModelIndex tl_idx = createIndex(row, 0);
    QModelIndex br_idx = createIndex(row, columnCount() - 1);
    emit dataChanged(tl_idx, br_idx);
  }
}

void RunTableModel::endResetIfPopulated()
{
  if (mCarsPopulated &&
      mCarClassesPopulated &&
      mDriversPopulated &&
      mRunsPopulated)
  {
    endResetModel();
  }
}

std::int64_t RunTableModel::indexRunId(const QModelIndex& index) const
{
  std::shared_ptr<DataItem> p_item = itemFromIndex(index);
  if (p_item == nullptr || p_item->mpRun == nullptr)
  {
    return -1;
  }
  return p_item->mpRun->run_id();
}

AUTOXTIME_UI_NAMESPACE_END
