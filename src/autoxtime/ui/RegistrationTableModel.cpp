#include <autoxtime/ui/RegistrationTableModel.h>

// TODO
// - integrate this class into registration widget with proxy model, delegate, etc

#include <autoxtime/db/CarModel.h>
#include <autoxtime/db/CarClassModel.h>
#include <autoxtime/db/DriverModel.h>
#include <autoxtime/db/EventModel.h>
#include <autoxtime/db/EventRegistrationModel.h>
#include <autoxtime/log/Log.h>
#include <autoxtime/proto/car.pb.h>
#include <autoxtime/proto/car_class.pb.h>
#include <autoxtime/proto/driver.pb.h>
#include <autoxtime/proto/event.pb.h>
#include <autoxtime/proto/event_registration.pb.h>
#include <autoxtime/ui/CheckBoxItemDelegate.h>
#include <autoxtime/ui/MultiSortFilterProxyModel.h>
#include <autoxtime/ui/StandardItem.h>
#include <autoxtime/util/ColorUtil.h>

#include <QComboBox>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QSplitter>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QTableView>
#include <QToolButton>

using autoxtime::db::CarModel;
using autoxtime::db::CarClassModel;
using autoxtime::db::DriverModel;
using autoxtime::db::EventModel;
using autoxtime::db::EventRegistrationModel;

AUTOXTIME_UI_NAMESPACE_BEG

const std::unordered_map<RegistrationTableModel::TableColumn, QString> RegistrationTableModel::COLUMN_HEADER_MAP = {
  { TABLE_COLUMN_FIRST_NAME,  "First Name" },
  { TABLE_COLUMN_LAST_NAME, "Last Name" },
  { TABLE_COLUMN_CLASS, "Class" },
  { TABLE_COLUMN_CAR_NUM, "Number" },
  { TABLE_COLUMN_CAR_COLOR, "Color" },
  { TABLE_COLUMN_CAR_MODEL, "Car Model" },
  { TABLE_COLUMN_CHECKED_IN, "✓ In"}
};

RegistrationTableModel::RegistrationTableModel(QWidget* pParent)
    : QAbstractTableModel(pParent),
      mpCarClassModel(new autoxtime::db::CarClassModel(this)),
      mpCarModel(new autoxtime::db::CarModel(this)),
      mpDriverModel(new autoxtime::db::DriverModel(this)),
      mpEventRegistrationModel(new autoxtime::db::EventRegistrationModel(this)),
      mCarsPopulated(false),
      mCarClassesPopulated(false),
      mDriversPopulated(false),
      mEventRegistrationsPopulated(false),
      mEventId(-1),
      mNumCheckedInEntries(0)
{
  // model signals
  connect(mpCarModel, &autoxtime::db::CarModel::findResult,
          this,       &RegistrationTableModel::setCars);
  connect(mpCarModel, &autoxtime::db::CarModel::notification,
          this,       &RegistrationTableModel::carNotification);

  connect(mpCarClassModel, &autoxtime::db::CarClassModel::findResult,
          this,            &RegistrationTableModel::setCarClasses);
  connect(mpCarClassModel, &autoxtime::db::CarClassModel::notification,
          this,            &RegistrationTableModel::carClassNotification);

  connect(mpDriverModel, &autoxtime::db::DriverModel::findResult,
          this,          &RegistrationTableModel::setDrivers);
  connect(mpDriverModel, &autoxtime::db::DriverModel::notification,
          this,          &RegistrationTableModel::driverNotification);

  connect(mpEventRegistrationModel, &autoxtime::db::EventRegistrationModel::findResult,
          this,                     &RegistrationTableModel::setEventRegistrations);
  connect(mpEventRegistrationModel, &autoxtime::db::EventRegistrationModel::notification,
          this,                     &RegistrationTableModel::eventRegistrationNotification);

  // notifications
  mpCarModel->subscribeToNotifications();
  mpCarClassModel->subscribeToNotifications();
  mpDriverModel->subscribeToNotifications();
  mpEventRegistrationModel->subscribeToNotifications();
}

////////////////////////////////////////////////////////////////////////////////
// BEGIN Qt Model Functions

// When subclassing QAbstractTableModel, you must implement rowCount(), columnCount(), and data().
// Well behaved models will also implement headerData().
// Editable models need to implement setData(), and implement flags() to return a value containing Qt::ItemIsEditable.

int RegistrationTableModel::columnCount(const QModelIndex& parent) const
{
  return COLUMN_HEADER_MAP.size();
}

int RegistrationTableModel::rowCount(const QModelIndex& parent) const
{
  return mDataItems.size();
}

std::shared_ptr<RegistrationTableModel::DataItem> RegistrationTableModel
::itemFromIndex(const QModelIndex& index) const
{
  if (index.row() < 0 || index.row() >= rowCount() ||
      index.column() < 0 || index.column() >= columnCount())
  {
    return std::shared_ptr<DataItem>();
  }
  return mDataItems[index.row()];
}

QVariant RegistrationTableModel::data(const QModelIndex& index, int role) const
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
    case TABLE_COLUMN_FIRST_NAME:
      if (p_item->mpDriver != nullptr && role == Qt::DisplayRole)
      {
        data = QVariant(QString::fromStdString(p_item->mpDriver->first_name().data()));
      }
      break;
    case TABLE_COLUMN_LAST_NAME:
      if (p_item->mpDriver != nullptr && role == Qt::DisplayRole)
      {
        data = QVariant(QString::fromStdString(p_item->mpDriver->last_name()));
      }
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
        data = QVariant::fromValue(p_item->mpCar->car_number());
      }
      break;
    case TABLE_COLUMN_CAR_COLOR:
      if (p_item->mpCar == nullptr) { break; }
      switch (role)
      {
        case Qt::DisplayRole:
          {
            QString color_name = QString::fromStdString(p_item->mpCar->color());
            if (p_item->mCarColor.isValid())
            {
              data = QVariant(color_name);
            }
            else
            {
              // unknown color
              data = QVariant("??? " + color_name);
            }
          }
          break;
        case Qt::BackgroundRole:
          if (p_item->mCarColor.isValid())
          {
            data = QVariant(QBrush(p_item->mCarColor));
          }
          break;
        default:
          break;
      }
      break;
    case TABLE_COLUMN_CAR_MODEL:
      if (p_item->mpCar != nullptr && role == Qt::DisplayRole)
      {
        data = QVariant(QString::fromStdString(p_item->mpCar->model()));
      }
      break;
    case TABLE_COLUMN_CHECKED_IN:
      if (p_item->mpEventRegistration == nullptr) { return QVariant(); }
      switch (role)
      {
        case Qt::DisplayRole:
          data = QVariant::fromValue(p_item->mpEventRegistration->checked_in());
          break;
        case Qt::CheckStateRole:
          data = QVariant::fromValue(p_item->mpEventRegistration->checked_in() ?
                                     Qt::Checked :
                                     Qt::Unchecked);
        default:
          break;
      }
      break;
  }
  return data;
}

QVariant RegistrationTableModel::headerData(int section,
                                            Qt::Orientation orientation,
                                            int role) const
{
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
  {
    return QVariant(COLUMN_HEADER_MAP.at(static_cast<TableColumn>(section)));
  }
  return QVariant();
}

bool RegistrationTableModel::setData(const QModelIndex& index,
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
    case TABLE_COLUMN_FIRST_NAME:
      // driver
      {
        if (p_item->mpDriver != nullptr)
        {
          p_item->mpDriver->set_first_name(value.toString().toStdString());
          mpDriverModel->update(*p_item->mpDriver);
          b_updated = true;
        }
      }
      break;
    case TABLE_COLUMN_LAST_NAME:
      // driver
      if (p_item->mpDriver != nullptr)
      {
        p_item->mpDriver->set_last_name(value.toString().toStdString());
        mpDriverModel->update(*p_item->mpDriver);
        b_updated = true;
      }
      break;
    case TABLE_COLUMN_CLASS:
      // carclass
      {
        // TODO find a new car class by name
      }
      break;
    case TABLE_COLUMN_CAR_NUM:
      // car
      if (p_item->mpCar != nullptr)
      {
        p_item->mpCar->set_car_number(value.toLongLong());
        mpCarModel->update(*p_item->mpCar);
        b_updated = true;
      }
      break;
    case TABLE_COLUMN_CAR_COLOR:
      // car
      if (p_item->mpCar != nullptr)
      {
        QString color_name = value.toString();
        // car color is special because we have to mess with the brush
        QColor color = autoxtime::util::ColorUtil::nameToColor(color_name);
        p_item->mCarColor = color;
        p_item->mpCar->set_color(color_name.toStdString());
        mpCarModel->update(*p_item->mpCar);
        b_updated = true;
      }
      break;
    case TABLE_COLUMN_CAR_MODEL:
      // car
      if (p_item->mpCar != nullptr)
      {
        AXT_DEBUG << "Setting car model data_str="
                  << value.toString()
                  << " valid=" << value.isValid()
                  << " role="
                  << role;
        p_item->mpCar->set_model(value.toString().toStdString());
        mpCarModel->update(*p_item->mpCar);
        b_updated = true;
      }
      break;
    case TABLE_COLUMN_CHECKED_IN:
      // event registration
      if (p_item->mpEventRegistration != nullptr)
      {
        AXT_DEBUG << "Setting checked in column data_str="
                  << value.toString()
                  << " data_bool="
                  << value.toBool();
        p_item->mpEventRegistration->set_checked_in(value.toBool());
        mpEventRegistrationModel->update(*p_item->mpEventRegistration);
        b_updated = true;
        if (value.toBool())
        {
          ++mNumCheckedInEntries;
        }
        else
        {
          --mNumCheckedInEntries;
        }
        emit numEntriesChanged(mEventRegistrations.size(),
                               mNumCheckedInEntries);
      }
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

Qt::ItemFlags RegistrationTableModel::flags(const QModelIndex &index) const
{
  return Qt::ItemIsSelectable
      |Qt::ItemIsEnabled
      |Qt::ItemIsEditable
      |Qt::ItemIsDragEnabled
      |Qt::ItemIsDropEnabled;
}

// END Qt Model Functions
////////////////////////////////////////////////////////////////////////////////

void RegistrationTableModel::setEventId(std::int64_t eventId)
{
  mEventId = eventId;

  // clear current table, only do this when event changes
  reset();

  // find all event registrations, async SQL query
  autoxtime::proto::EventRegistration er;
  er.set_event_id(mEventId);
  mpEventRegistrationModel->findAsync(er);
}

void RegistrationTableModel::reset()
{
  beginResetModel();

  // clear our underlying state
  mCars.clear();
  mCarClasses.clear();
  mDrivers.clear();
  mEventRegistrations.clear();
  mDataItems.clear();

  mCarsPopulated = false;
  mCarClassesPopulated = false;
  mDriversPopulated = false;
  mEventRegistrationsPopulated = false;

  endResetModel();
}

void RegistrationTableModel
::setEventRegistrations(const std::vector<std::shared_ptr<autoxtime::proto::EventRegistration>>& eventRegistrations)
{
  beginResetModel();

  AXT_DEBUG << "Found " << eventRegistrations.size() << " registrations for event id: "
            << mEventId;
  for (const std::shared_ptr<autoxtime::proto::EventRegistration>& p_er : eventRegistrations)
  {
    updateEventRegistration(p_er);
  }

  // TODO add all of the event IDs, driver ids, etc so we can match them later
  std::string bind_event_id = ":event_id";
  std::unordered_map<QString, QVariant> bindings;
  bindings[bind_event_id.data()] = QVariant::fromValue(mEventId);

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
  mEventRegistrationsPopulated = true;
}

bool RegistrationTableModel
::updateEventRegistration(const std::shared_ptr<autoxtime::proto::EventRegistration>& pEventRegistration)
{
  // get current event ID and use it for binding in SQL queries
  if (pEventRegistration->event_id() != mEventId)
  {
    return false;
  }

  std::int64_t id = pEventRegistration->event_registration_id();
  std::unordered_map<std::int64_t, std::shared_ptr<DataItem>>::iterator iter =
      mEventRegistrations.find(id);

  std::shared_ptr<DataItem> p_item;
  bool b_new = false;
  if (iter != mEventRegistrations.end())
  {
    AXT_DEBUG << "event registration id exists in the map: " << id;
    // mapping from ID -> item already exists
    p_item = iter->second;
  }
  else
  {
    AXT_DEBUG << "event registration id doesn't exists in the map: " << id;
    int row_idx = mDataItems.size();
    beginInsertRows(QModelIndex(), row_idx, row_idx);
    p_item = std::make_shared<DataItem>();
    p_item->row = row_idx;
    mDataItems.push_back(p_item);
    // add mapping for this item
    mEventRegistrations[id] = p_item;
    b_new = true;
  }

  // map related IDs to this item
  AXT_DEBUG << "Adding driver id: " << pEventRegistration->driver_id();
  mDrivers[pEventRegistration->driver_id()] = p_item;
  AXT_DEBUG << "Adding car id: " << pEventRegistration->car_id();
  mCars[pEventRegistration->car_id()] = p_item;

  Qt::CheckState old_check_state = Qt::Unchecked;
  if (p_item->mpEventRegistration != nullptr)
  {
    old_check_state = p_item->mpEventRegistration->checked_in() ? Qt::Checked : Qt::Unchecked;
  }
  Qt::CheckState new_check_state =
      pEventRegistration->checked_in() ? Qt::Checked : Qt::Unchecked;
  // only modify checked in entries when checked state changes
  if (old_check_state != new_check_state)
  {
    if (pEventRegistration->checked_in())
    {
      ++mNumCheckedInEntries;
    }
    else
    {
      --mNumCheckedInEntries;
    }
  }

  // update our event registration point to contain the new data
  p_item->mpEventRegistration = pEventRegistration;
  emit numEntriesChanged(mDataItems.size(),
                         mNumCheckedInEntries);

  if (b_new)
  {
    endInsertRows();
  }
  itemDataChanged(p_item);
  return true;
}

void RegistrationTableModel
::setDrivers(const std::vector<std::shared_ptr<autoxtime::proto::Driver>>& drivers)
{
  AXT_DEBUG << "Found " << drivers.size() << " drivers for event id: "
            << mEventId;
  for (const std::shared_ptr<autoxtime::proto::Driver>& p_driver : drivers)
  {
    if (!updateDriver(p_driver))
    {
      AXT_ERROR << "A Driver came back without a corresponding event registration: "
                << p_driver->driver_id();
    }
  }
  mDriversPopulated = true;
}

bool RegistrationTableModel
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
  return false;
}

void RegistrationTableModel
::setCars(const std::vector<std::shared_ptr<autoxtime::proto::Car>>& cars)
{
  AXT_DEBUG << "Found " << cars.size() << " cars for event id: "
            << mEventId;

  for (const std::shared_ptr<autoxtime::proto::Car>& p_car : cars)
  {
    if (!updateCar(p_car))
    {
      AXT_ERROR << "A car came back without a corresponding event registration: "
                << p_car->car_id();
    }
  }

  // update car classes in case they came back before the cars did
  updateCarClasses();
  mCarsPopulated = true;
}

bool RegistrationTableModel
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

    // car color is special because we have to mess with the brush
    QString color_name = QString::fromStdString(pCar->color());
    QColor color = autoxtime::util::ColorUtil::nameToColor(color_name);
    iter->second->mCarColor = color;

    itemDataChanged(p_item);
    return true;
  }
  return false;
}

void RegistrationTableModel
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

void RegistrationTableModel::updateCarClasses()
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

bool RegistrationTableModel
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
  return false;
}


void RegistrationTableModel::carNotification(const std::shared_ptr<google::protobuf::Message>& pMessage,
                                              const QDateTime& timestamp,
                                              const QString& operation)
{
  AXT_DEBUG << "RegistrationTableModel - car notification";
  std::shared_ptr<autoxtime::proto::Car> p_car = CarModel::messageToT(pMessage);
  // only update if we care about it
  if (mCars.find(p_car->car_id()) != mCars.end())
  {
    updateCar(p_car);
  }
}

void RegistrationTableModel::carClassNotification(const std::shared_ptr<google::protobuf::Message>& pMessage,
                                                   const QDateTime& timestamp,
                                                   const QString& operation)
{
  AXT_DEBUG << "RegistrationTableModel - car class notification";
  // only update if we care about it
  std::shared_ptr<autoxtime::proto::CarClass> p_car_class = CarClassModel::messageToT(pMessage);
  if (mCarClasses.find(p_car_class->car_class_id()) != mCarClasses.end())
  {
    updateCarClass(p_car_class);
  }
}

void RegistrationTableModel::driverNotification(const std::shared_ptr<google::protobuf::Message>& pMessage,
                                                 const QDateTime& timestamp,
                                                 const QString& operation)
{
  AXT_DEBUG << "RegistrationTableModel - driver notification " << operation << "\n"
            << pMessage->DebugString();
  std::shared_ptr<autoxtime::proto::Driver> p_driver = DriverModel::messageToT(pMessage);
  // only update if we care about it
  if (mDrivers.find(p_driver->driver_id()) != mDrivers.end())
  {
    updateDriver(p_driver);
  }
}

void RegistrationTableModel::eventRegistrationNotification(const std::shared_ptr<google::protobuf::Message>& pMessage,
                                                           const QDateTime& timestamp,
                                                           const QString& operation)
{
  AXT_DEBUG << "RegistrationTableModel - event registration notification";
  std::shared_ptr<autoxtime::proto::EventRegistration> p_er =
      EventRegistrationModel::messageToT(pMessage);
  // only update if we care about it
  if (mEventRegistrations.find(p_er->event_registration_id()) != mEventRegistrations.end())
  {
    updateEventRegistration(p_er);
  }
}

void RegistrationTableModel::itemDataChanged(const std::shared_ptr<DataItem>& pItem)
{
  int row = pItem->row;
  QModelIndex tl_idx = createIndex(row, 0);
  QModelIndex br_idx = createIndex(row, columnCount() - 1);
  emit dataChanged(tl_idx, br_idx);
}

void RegistrationTableModel::endResetIfPopulated()
{
  if (mCarsPopulated &&
      mCarClassesPopulated &&
      mDriversPopulated &&
      mEventRegistrationsPopulated)
  {
    endResetModel();
  }
}

AUTOXTIME_UI_NAMESPACE_END
