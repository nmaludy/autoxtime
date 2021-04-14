#include <autoxtime/ui/RegistrationWidget.h>

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
#include <autoxtime/util/ColorUtil.h>

#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTableView>

using autoxtime::db::CarModel;
using autoxtime::db::CarClassModel;
using autoxtime::db::DriverModel;
using autoxtime::db::EventModel;
using autoxtime::db::EventRegistrationModel;

AUTOXTIME_UI_NAMESPACE_BEG

RegistrationWidget::RegistrationWidget(QWidget* pParent)
    : QWidget(pParent),
      mpCarClassModel(new autoxtime::db::CarClassModel(this)),
      mpCarModel(new autoxtime::db::CarModel(this)),
      mpDriverModel(new autoxtime::db::DriverModel(this)),
      mpEventModel(new autoxtime::db::EventModel(this)),
      mpEventRegistrationModel(new autoxtime::db::EventRegistrationModel(this)),
      mpEventComboBox(new QComboBox(this)),
      mpFilterLineEdit(new QLineEdit(this)),
      mpEventRegistrationTable(new QTableView(this)),
      mpEventItemModel(new QStandardItemModel(this)),
      mpEventSortFilterProxyModel(new MultiSortFilterProxyModel(this)),
      mTableRowAddIdx(0)
{
  QGridLayout* p_layout = new QGridLayout(this);
  // p_layout->addWidget(new QLabel("This will be some registration stuff"), 0, 0);

  // events combo box
  {
    // might want to change this later to not be alphabetical
    mpEventComboBox->setInsertPolicy(QComboBox::InsertAlphabetically);

    connect(mpEventComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,            &RegistrationWidget::eventComboIndexChanged);
    connect(mpEventModel, &autoxtime::db::EventModel::listResult,
            this,         &RegistrationWidget::setEvents);
    mpEventModel->listAsync();

    p_layout->addWidget(mpEventComboBox, 0, 0, 1, -1);
  }

  // filter
  {
    connect(mpFilterLineEdit, &QLineEdit::textChanged,
            this,             &RegistrationWidget::filterChanged);

    p_layout->addWidget(new QLabel("Filter:"), 1, 0);
    p_layout->addWidget(mpFilterLineEdit, 1, 1, 1, -1);
  }

  // table
  {
    QList<int> columns;
    columns << TABLE_COLUMN_FIRST_NAME
            << TABLE_COLUMN_LAST_NAME
            << TABLE_COLUMN_CLASS
            << TABLE_COLUMN_CAR_NUM
            << TABLE_COLUMN_CAR;
    mpEventSortFilterProxyModel->setFilterKeyColumnList(columns);
    mpEventSortFilterProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    mpEventSortFilterProxyModel->setSourceModel(mpEventItemModel);
    mpEventRegistrationTable->setModel(mpEventSortFilterProxyModel);

    mpEventRegistrationTable->setItemDelegateForColumn(TABLE_COLUMN_CHECKED_IN,
                                                       new CheckBoxItemDelegate(this));

    resetTable();
    p_layout->addWidget(mpEventRegistrationTable, 2, 0, -1, -1);
  }

  // model signals
  connect(mpCarModel, &autoxtime::db::CarModel::findResult,
          this,       &RegistrationWidget::setCars);
  connect(mpCarClassModel, &autoxtime::db::CarClassModel::findResult,
          this,            &RegistrationWidget::setCarClasses);
  connect(mpDriverModel, &autoxtime::db::DriverModel::findResult,
          this,          &RegistrationWidget::setDrivers);
  connect(mpEventRegistrationModel, &autoxtime::db::EventRegistrationModel::findResult,
          this,                     &RegistrationWidget::setEventRegistrations);
}

void RegistrationWidget::resetTable()
{
  mpEventItemModel->clear();

  // clear our underlying state
  mTableRowAddIdx = 0;
  mCars.clear();
  mCarClasses.clear();
  mDrivers.clear();
  mEventRegistrations.clear();

  mCarItems.clear();
  mDriverItems.clear();
  mEventRegistrationItems.clear();

  QStringList headers;
  headers << "First Name"
          << "Last Name"
          << "Class"
          << "Number"
          << " "
          << "Car"
          << "Checked-in";
  mpEventItemModel->setRowCount(0);
  mpEventItemModel->setColumnCount(headers.size());
  mpEventItemModel->setHorizontalHeaderLabels(headers);

  AXT_DEBUG << "Rows in table after clear: " << mpEventItemModel->rowCount();
}

void RegistrationWidget::setEvents(const autoxtime::db::EventModel::ProtoPtrVec& events)
{
  mpEventComboBox->clear();
  for (const std::shared_ptr<autoxtime::proto::Event>& p_event : events)
  {
    mpEventComboBox->addItem(QString::fromStdString(p_event->name()),
                             QVariant::fromValue(p_event->event_id()));
  }
}

void RegistrationWidget::eventComboIndexChanged(int index)
{
  if (index == -1)
  {
    AXT_DEBUG << "Event combo box reset or cleared";
    return;
  }
  // get current event ID and use it for binding in SQL queries
  std::int64_t event_id = mpEventComboBox->currentData().toLongLong();

  // clear current table, only do this when event changes
  resetTable();

  // find all event registrations, async SQL query
  autoxtime::proto::EventRegistration er;
  er.set_event_id(event_id);
  mpEventRegistrationModel->findAsync(er);
}

void RegistrationWidget::filterChanged(const QString& text)
{
  mpEventSortFilterProxyModel->setFilterWildcard(text);
}

void RegistrationWidget
::setEventRegistrations(const std::vector<std::shared_ptr<autoxtime::proto::EventRegistration>>& eventRegistrations)
{
  mEventRegistrations = eventRegistrations;
  std::int64_t event_id = mpEventComboBox->currentData().toLongLong();
  AXT_DEBUG << "Found " << eventRegistrations.size() << " registrations for event id: "
            << event_id;

  // _MUST_ stop sorting before messing with data, otherwise the "row" value
  // could change as we mess with the table items (annoying, but it's OK)
  mpEventRegistrationTable->setSortingEnabled(false);
  for (const std::shared_ptr<autoxtime::proto::EventRegistration>& p_er : mEventRegistrations)
  {
    // do we know about this driver yet or not?
    std::int64_t id = p_er->event_registration_id();
    std::unordered_map<std::int64_t, QStandardItem*>::iterator iter =
        mEventRegistrationItems.find(id);

    int row_idx = -1;
    if (iter != mEventRegistrationItems.end())
    {
      AXT_DEBUG << "event registration id exists in the map: " << id;
      // convert the item into a row index using the table's mapping
      QModelIndex idx = mpEventItemModel->indexFromItem(iter->second);
      row_idx = idx.row();
    }
    else
    {
      AXT_DEBUG << "event registration id doesn't exists in the map: " << id;
      // add a new row at the end of the table
      row_idx = mpEventItemModel->rowCount();
      mpEventItemModel->insertRow(row_idx);

      // add all of the item columns
      mpEventItemModel->setItem(row_idx, TABLE_COLUMN_FIRST_NAME,
                                new QStandardItem());
      mpEventItemModel->setItem(row_idx, TABLE_COLUMN_LAST_NAME,
                                new QStandardItem());
      mpEventItemModel->setItem(row_idx, TABLE_COLUMN_CLASS,
                                new QStandardItem());
      mpEventItemModel->setItem(row_idx, TABLE_COLUMN_CAR_NUM,
                                new QStandardItem());
      mpEventItemModel->setItem(row_idx, TABLE_COLUMN_CAR_COLOR,
                                new QStandardItem());
      mpEventItemModel->setItem(row_idx, TABLE_COLUMN_CAR,
                                new QStandardItem());
      mpEventItemModel->setItem(row_idx, TABLE_COLUMN_CHECKED_IN,
                                new QStandardItem());
    }

    // keep track of FIRST_NAME column, by convention
    QStandardItem* p_item =
        mpEventItemModel->item(row_idx, TABLE_COLUMN_FIRST_NAME);

    // keep IDs in the item of the FIRST_NAME column
    p_item->setData(QVariant::fromValue(p_er->event_registration_id()),
                    TABLE_ROLE_EVENT_REGISTRATION_ID);
    p_item->setData(QVariant::fromValue(p_er->driver_id()),
                    TABLE_ROLE_DRIVER_ID);
    p_item->setData(QVariant::fromValue(p_er->car_id()),
                    TABLE_ROLE_CAR_ID);

    // keep track of mappings from ID -> item
    mDriverItems[p_er->driver_id()] = p_item;
    mEventRegistrationItems[p_er->event_registration_id()] = p_item;
    mCarItems[p_er->car_id()] = p_item;

    // set data from registration object
    p_item = mpEventItemModel->item(row_idx, TABLE_COLUMN_CHECKED_IN);
    // p_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
    p_item->setCheckState(p_er->checked_in() ? Qt::Checked : Qt::Unchecked);
    // TODO capture checked signal and send it somewhere
  }
  mpEventRegistrationTable->setSortingEnabled(true);

  // TODO add all of the event IDs, driver ids, etc so we can match them later
  std::string bind_event_id = ":event_id";
  std::unordered_map<QString, QVariant> bindings;
  bindings[bind_event_id.data()] = QVariant::fromValue(event_id);

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
}

void RegistrationWidget
::setDrivers(const std::vector<std::shared_ptr<autoxtime::proto::Driver>>& drivers)
{
  mDrivers = drivers;
  std::int64_t event_id = mpEventComboBox->currentData().toLongLong();
  AXT_DEBUG << "Found " << drivers.size() << " drivers for event id: "
            << event_id;

  // _MUST_ stop sorting before messing with data, otherwise the "row" value
  // could change as we mess with the table items (annoying, but it's OK)
  mpEventRegistrationTable->setSortingEnabled(false);

  for (const std::shared_ptr<autoxtime::proto::Driver>& p_driver : mDrivers)
  {
    // do we know about this driver yet or not?
    std::int64_t id = p_driver->driver_id();
    std::unordered_map<std::int64_t, QStandardItem*>::iterator iter =
        mDriverItems.find(id);

    if (iter != mDriverItems.end())
    {
      // AXT_DEBUG << "driver id exists in the map: " << id;
      // convert the item into a row index using the table's mapping
      QModelIndex idx = mpEventItemModel->indexFromItem(iter->second);
      int row_idx = idx.row();

      // update existing items with data
      // add this items id to the FIST_NAME column, by convention
      updateItem(row_idx, TABLE_COLUMN_FIRST_NAME,
                 QString::fromStdString(p_driver->first_name()));
      updateItem(row_idx, TABLE_COLUMN_LAST_NAME,
                 QString::fromStdString(p_driver->last_name()));
    }
    else
    {
      AXT_ERROR << "A Driver came back without a corresponding event registration: " << id;
      continue;
    }
  }
  mpEventRegistrationTable->setSortingEnabled(true);
}

void RegistrationWidget::updateItem(int row,
                                    int col,
                                    QString text,
                                    int dataRole,
                                    QVariant data)
{
  QStandardItem* p_item = mpEventItemModel->item(row, col);
  if (p_item)
  {
    if (!text.isNull())
    {
      p_item->setText(text);
    }
    if (!data.isNull())
    {
      p_item->setData(data, dataRole);
    }
  }
  else
  {
    AXT_ERROR << "updateItem() - table cell doesn't exist: "
              << " row = " << row
              << " col = " << col;
  }
}

void RegistrationWidget
::setCars(const std::vector<std::shared_ptr<autoxtime::proto::Car>>& cars)
{
  mCars = cars;
  std::int64_t event_id = mpEventComboBox->currentData().toLongLong();
  AXT_DEBUG << "Found " << cars.size() << " cars for event id: "
            << event_id;

  // _MUST_ stop sorting before messing with data, otherwise the "row" value
  // could change as we mess with the table items (annoying, but it's OK)
  mpEventRegistrationTable->setSortingEnabled(false);

  for (const std::shared_ptr<autoxtime::proto::Car>& p_car : mCars)
  {
    // do we know about this yet or not?
    std::int64_t id = p_car->car_id();
    std::unordered_map<std::int64_t, QStandardItem*>::iterator iter =
        mCarItems.find(id);

    if (iter != mCarItems.end())
    {
      // AXT_DEBUG << "car id exists in the map: " << id;
      // convert the item into a row index using the table's mapping
      QModelIndex idx = mpEventItemModel->indexFromItem(iter->second);
      int row_idx = idx.row();
      updateItem(row_idx, TABLE_COLUMN_CAR,
                 QString::fromStdString(p_car->make()
                                        + " "
                                        + p_car->model()));
      updateItem(row_idx, TABLE_COLUMN_CAR_NUM,
                 QString("%1").arg(p_car->car_number()));

      // car color is special because we have to mess with the brush
      QStandardItem* p_item = mpEventItemModel->item(row_idx, TABLE_COLUMN_CAR_COLOR);
      if (p_item)
      {
        QString color_name = QString::fromStdString(p_car->color());
        QColor color = autoxtime::util::ColorUtil::nameToColor(color_name);
        if (color.isValid())
        {
          p_item->setBackground(QBrush(color));
          p_item->setText(color_name);
        }
        else
        {
          // unknown color
          p_item->setText("??? " + color_name);
        }
      }

      // add data to the first name column about the class id
      updateItem(row_idx, TABLE_COLUMN_FIRST_NAME,
                 QString(),
                 TABLE_ROLE_CAR_CLASS_ID,
                 QVariant::fromValue(p_car->car_class_id()));
    }
    else
    {
      AXT_ERROR << "A car came back without a corresponding event registration: " << id;
      continue;
    }
  }
  mpEventRegistrationTable->setSortingEnabled(true);

  // update car classes in case they came back before the cars did
  updateCarClasses();
}

void RegistrationWidget
::setCarClasses(const std::vector<std::shared_ptr<autoxtime::proto::CarClass>>& carClasses)
{
  std::int64_t event_id = mpEventComboBox->currentData().toLongLong();
  AXT_DEBUG << "Found " << carClasses.size() << " cars classes for event id: "
            << event_id;
  mCarClasses.clear();
  for (const std::shared_ptr<autoxtime::proto::CarClass>& p_cc : carClasses)
  {
    mCarClasses[p_cc->car_class_id()] = p_cc;
  }

  // update car classes in case they came back after the cars did
  updateCarClasses();
}

void RegistrationWidget::updateCarClasses()
{
  AXT_DEBUG << "updateCarClasses - begin";
  if (mCars.empty() || mCarItems.empty() || mCarClasses.empty())
  {
    AXT_DEBUG << "updateCarClasses() - cars or classes are empty";
    return;
  }
  AXT_DEBUG << "updateCarClasses() - cars size = " << mCarItems.size()
            << " car classes size = " << mCarClasses.size();

  mpEventRegistrationTable->setSortingEnabled(false);
  for (std::pair<const std::int64_t, QStandardItem*>& id_item : mCarItems)
  {
    QStandardItem* p_item = id_item.second;
    QVariant data = p_item->data(TABLE_ROLE_CAR_CLASS_ID);
    if (data.isNull())
    {
      AXT_DEBUG << "upcateCarClasses() - Data is null";
      continue;
    }

    // lookup the car class by ID to get its name
    std::int64_t car_class_id = data.toLongLong();
    std::unordered_map<std::int64_t, std::shared_ptr<autoxtime::proto::CarClass>>::iterator iter =
        mCarClasses.find(car_class_id);
    if (iter == mCarClasses.end())
    {
      AXT_DEBUG << "updateCarClasses() - couldn't find car class with id: " << car_class_id;
      continue;
    }

    // get the name of the car class
    std::shared_ptr<autoxtime::proto::CarClass>& p_cc = iter->second;
    std::string name = p_cc->name();
    if (p_cc->has_subclass_name() && !p_cc->subclass_name().empty())
    {
      name += "-" + p_cc->subclass_name();
    }

    // add text to the item
    QModelIndex idx = mpEventItemModel->indexFromItem(p_item);
    int row_idx = idx.row();
    updateItem(row_idx, TABLE_COLUMN_CLASS,
               QString::fromStdString(name));
  }
  mpEventRegistrationTable->setSortingEnabled(true);
  AXT_DEBUG << "updateCarClasses - done";
}

AUTOXTIME_UI_NAMESPACE_END
