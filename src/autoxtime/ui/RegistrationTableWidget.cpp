#include <autoxtime/ui/RegistrationTableWidget.h>

#include <autoxtime/log/Log.h>
#include <autoxtime/ui/CheckBoxItemDelegate.h>
#include <autoxtime/ui/MultiSortFilterProxyModel.h>
#include <autoxtime/ui/RegistrationTableModel.h>

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

AUTOXTIME_UI_NAMESPACE_BEG

RegistrationTableWidget::RegistrationTableWidget(QWidget* pParent)
    : QWidget(pParent),
      mpFilterLineEdit(new QLineEdit(this)),
      mpTableView(new QTableView(this)),
      mpRegistrationTableModel(new RegistrationTableModel(this)),
      mpSortFilterProxyModel(new MultiSortFilterProxyModel(this))
{
  QGridLayout* p_layout = new QGridLayout(this);
  // TODO make this configurable
  mTableColumnHeaders << "First Name"
                      << "Last Name"
                      << "Class"
                      << "Number"
                      << "Color"
                      << "Car Model"
                      << "✓ In";
  // TODO make this configurable
  mTableFilterColumns << RegistrationTableModel::TABLE_COLUMN_FIRST_NAME
                      << RegistrationTableModel::TABLE_COLUMN_LAST_NAME
                      << RegistrationTableModel::TABLE_COLUMN_CLASS
                      << RegistrationTableModel::TABLE_COLUMN_CAR_NUM
                      << RegistrationTableModel::TABLE_COLUMN_CAR_MODEL;

  // filter
  {
    connect(mpFilterLineEdit, &QLineEdit::textChanged,
            this,             &RegistrationTableWidget::filterChanged);

    QMenu* p_menu = new QMenu(this);
    for (int i = 0; i < mTableColumnHeaders.size(); ++i)
    {
      const QString& hdr = mTableColumnHeaders.at(i);
      QAction* p_action = p_menu->addAction(hdr);
      p_action->setData(i);
      p_action->setCheckable(true);
      p_action->setChecked(mTableFilterColumns.contains(i));
      mTableFilterColumnActions << p_action;
      connect(p_action, &QAction::toggled,
              this,     &RegistrationTableWidget::filterColumnToggled);
    }

    QPushButton* p_filter_button = new QPushButton("Filter", this);
    p_filter_button->setMenu(p_menu);
    p_layout->addWidget(p_filter_button, 0, 0);
    p_layout->addWidget(mpFilterLineEdit, 0, 1, 1, -1);
  }

  // table
  {
    mpSortFilterProxyModel->setFilterKeyColumnList(mTableFilterColumns);
    mpSortFilterProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    mpSortFilterProxyModel->setSourceModel(mpRegistrationTableModel);
    mpTableView->setModel(mpSortFilterProxyModel);

    // enable column headers
    mpTableView->horizontalHeader()->setVisible(true);
    // disable row numbers
    mpTableView->verticalHeader()->setVisible(false);
    // enable sorting
    mpTableView->setSortingEnabled(true);
    mpTableView->sortByColumn(RegistrationTableModel::TABLE_COLUMN_FIRST_NAME,
                              Qt::AscendingOrder);

    // setup check boxes
    mpTableView->setItemDelegateForColumn(RegistrationTableModel::TABLE_COLUMN_CHECKED_IN,
                                          new CheckBoxItemDelegate(this));

    // signals/slots
    connect(mpRegistrationTableModel, &RegistrationTableModel::numEntriesChanged,
            this,                     &RegistrationTableWidget::modelNumEntriesChanged);

    p_layout->addWidget(mpTableView, 1, 0, 1, -1);
    p_layout->setRowStretch(1, 1);

    // load data from database
    mpRegistrationTableModel->reset();
  }
}

void RegistrationTableWidget::setEventId(std::int64_t eventId)
{
  mpRegistrationTableModel->setEventId(eventId);
}

void RegistrationTableWidget::modelNumEntriesChanged(std::uint64_t numEntries,
                                                     std::uint64_t numCheckInEntries)
{
  emit numEntriesChanged(numEntries, numCheckInEntries);
}

void RegistrationTableWidget::filterChanged(const QString& text)
{
  mpSortFilterProxyModel->setFilterSmart(text);
}

void RegistrationTableWidget::filterColumnToggled()
{
  mTableFilterColumns.clear();
  for (int i = 0; i < mTableFilterColumnActions.size(); ++i)
  {
    QAction* p_action = mTableFilterColumnActions.at(i);
    if (p_action->isChecked())
    {
      mTableFilterColumns << i;
    }
  }
  mpSortFilterProxyModel->setFilterKeyColumnList(mTableFilterColumns);
}

AUTOXTIME_UI_NAMESPACE_END

// #include <autoxtime/ui/RegistrationTableWidget.h>

// #include <autoxtime/db/CarModel.h>
// #include <autoxtime/db/CarClassModel.h>
// #include <autoxtime/db/DriverModel.h>
// #include <autoxtime/db/EventModel.h>
// #include <autoxtime/db/EventRegistrationModel.h>
// #include <autoxtime/log/Log.h>
// #include <autoxtime/proto/car.pb.h>
// #include <autoxtime/proto/car_class.pb.h>
// #include <autoxtime/proto/driver.pb.h>
// #include <autoxtime/proto/event.pb.h>
// #include <autoxtime/proto/event_registration.pb.h>
// #include <autoxtime/ui/CheckBoxItemDelegate.h>
// #include <autoxtime/ui/MultiSortFilterProxyModel.h>
// #include <autoxtime/ui/StandardItem.h>
// #include <autoxtime/util/ColorUtil.h>

// #include <QComboBox>
// #include <QGridLayout>
// #include <QHeaderView>
// #include <QLabel>
// #include <QLineEdit>
// #include <QMenu>
// #include <QPushButton>
// #include <QSplitter>
// #include <QStandardItem>
// #include <QStandardItemModel>
// #include <QStatusBar>
// #include <QTableView>
// #include <QToolButton>

// using autoxtime::db::CarModel;
// using autoxtime::db::CarClassModel;
// using autoxtime::db::DriverModel;
// using autoxtime::db::EventModel;
// using autoxtime::db::EventRegistrationModel;

// AUTOXTIME_UI_NAMESPACE_BEG

// RegistrationTableWidget::RegistrationTableWidget(QWidget* pParent)
//     : QWidget(pParent),
//       mpCarClassModel(new autoxtime::db::CarClassModel(this)),
//       mpCarModel(new autoxtime::db::CarModel(this)),
//       mpDriverModel(new autoxtime::db::DriverModel(this)),
//       mpEventRegistrationModel(new autoxtime::db::EventRegistrationModel(this)),
//       mEventId(-1),
//       mpFilterLineEdit(new QLineEdit(this)),
//       mpEventRegistrationTable(new QTableView(this)),
//       mpEventItemModel(new QStandardItemModel(this)),
//       mpEventSortFilterProxyModel(new MultiSortFilterProxyModel(this)),
//       mTableRowAddIdx(0),
//       mNumCheckedInEntries(0)
// {
//   QGridLayout* p_layout = new QGridLayout(this);
//   mTableColumnHeaders << "First Name"
//                       << "Last Name"
//                       << "Class"
//                       << "Number"
//                       << "Color"
//                       << "Car Model"
//                       << "✓ In";

//   mTableFilterColumns << TABLE_COLUMN_FIRST_NAME
//                       << TABLE_COLUMN_LAST_NAME
//                       << TABLE_COLUMN_CLASS
//                       << TABLE_COLUMN_CAR_NUM
//                       << TABLE_COLUMN_CAR_MODEL;

//   // filter
//   {
//     connect(mpFilterLineEdit, &QLineEdit::textChanged,
//             this,             &RegistrationTableWidget::filterChanged);

//     QMenu* p_menu = new QMenu(this);
//     for (int i = 0; i < mTableColumnHeaders.size(); ++i)
//     {
//       const QString& hdr = mTableColumnHeaders.at(i);
//       QAction* p_action = p_menu->addAction(hdr);
//       p_action->setData(i);
//       p_action->setCheckable(true);
//       p_action->setChecked(mTableFilterColumns.contains(i));
//       mTableFilterColumnActions << p_action;
//       connect(p_action, &QAction::toggled,
//               this,     &RegistrationTableWidget::filterColumnToggled);
//     }

//     QPushButton* p_filter_button = new QPushButton("Filter", this);
//     p_filter_button->setMenu(p_menu);
//     p_layout->addWidget(p_filter_button, 0, 0);
//     p_layout->addWidget(mpFilterLineEdit, 0, 1, 1, -1);
//   }

//   // table
//   {
//     mpEventSortFilterProxyModel->setFilterKeyColumnList(mTableFilterColumns);
//     mpEventSortFilterProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
//     mpEventSortFilterProxyModel->setSourceModel(mpEventItemModel);
//     mpEventRegistrationTable->setModel(mpEventSortFilterProxyModel);

//     // disable row numbers
//     // mpEventRegistrationTable->verticalHeader()->setVisible(false);

//     // setup check boxes
//     mpEventRegistrationTable->setItemDelegateForColumn(TABLE_COLUMN_CHECKED_IN,
//                                                        new CheckBoxItemDelegate(this));

//     connect(mpEventSortFilterProxyModel, &QSortFilterProxyModel::dataChanged,
//             this,                        &RegistrationTableWidget::modelDataChanged);

//     p_layout->addWidget(mpEventRegistrationTable, 1, 0, 1, -1);
//     p_layout->setRowStretch(1, 1);
//     resetTable();
//   }

//   // model signals
//   connect(mpCarModel, &autoxtime::db::CarModel::findResult,
//           this,       &RegistrationTableWidget::setCars);
//   connect(mpCarModel, &autoxtime::db::CarModel::notification,
//           this,       &RegistrationTableWidget::carNotification);

//   connect(mpCarClassModel, &autoxtime::db::CarClassModel::findResult,
//           this,            &RegistrationTableWidget::setCarClasses);
//   connect(mpCarClassModel, &autoxtime::db::CarClassModel::notification,
//           this,            &RegistrationTableWidget::carClassNotification);

//   connect(mpDriverModel, &autoxtime::db::DriverModel::findResult,
//           this,          &RegistrationTableWidget::setDrivers);
//   connect(mpDriverModel, &autoxtime::db::DriverModel::notification,
//           this,          &RegistrationTableWidget::driverNotification);

//   connect(mpEventRegistrationModel, &autoxtime::db::EventRegistrationModel::findResult,
//           this,                     &RegistrationTableWidget::setEventRegistrations);
//   connect(mpEventRegistrationModel, &autoxtime::db::EventRegistrationModel::notification,
//           this,                     &RegistrationTableWidget::eventRegistrationNotification);

//   // notifications
//   mpCarModel->subscribeToNotifications();
//   mpCarClassModel->subscribeToNotifications();
//   mpDriverModel->subscribeToNotifications();
//   mpEventRegistrationModel->subscribeToNotifications();
// }

// void RegistrationTableWidget::resetTable()
// {
//   mpEventItemModel->clear();

//   // clear our underlying state
//   mTableRowAddIdx = 0;
//   mCars.clear();
//   mCarClasses.clear();
//   mDrivers.clear();
//   mEventRegistrations.clear();

//   mCarItems.clear();
//   mDriverItems.clear();
//   mEventRegistrationItems.clear();

//   mpEventItemModel->setRowCount(0);
//   mpEventItemModel->setColumnCount(mTableColumnHeaders.size());
//   mpEventItemModel->setHorizontalHeaderLabels(mTableColumnHeaders);
//   mpEventRegistrationTable->resizeColumnsToContents();

//   AXT_DEBUG << "Rows in table aftter clear: " << mpEventItemModel->rowCount();
// }


// void RegistrationTableWidget::setEventId(std::int64_t eventId)
// {
//   mEventId = eventId;

//   // clear current table, only do this when event changes
//   resetTable();

//   // find all event registrations, async SQL query
//   autoxtime::proto::EventRegistration er;
//   er.set_event_id(mEventId);
//   mpEventRegistrationModel->findAsync(er);
// }

// void RegistrationTableWidget::filterChanged(const QString& text)
// {
//   mpEventSortFilterProxyModel->setFilterSmart(text);
// }

// void RegistrationTableWidget::filterColumnToggled()
// {
//   mTableFilterColumns.clear();
//   for (int i = 0; i < mTableFilterColumnActions.size(); ++i)
//   {
//     QAction* p_action = mTableFilterColumnActions.at(i);
//     if (p_action->isChecked())
//     {
//       mTableFilterColumns << i;
//     }
//   }
//   mpEventSortFilterProxyModel->setFilterKeyColumnList(mTableFilterColumns);
// }

// void RegistrationTableWidget
// ::setEventRegistrations(const std::vector<std::shared_ptr<autoxtime::proto::EventRegistration>>& eventRegistrations)
// {
//   mEventRegistrations.clear();
//   AXT_DEBUG << "Found " << eventRegistrations.size() << " registrations for event id: "
//             << mEventId;

//   // _MUST_ stop sorting before messing with data, otherwise the "row" value
//   // could change as we mess with the table items (annoying, but it's OK)
//   mpEventRegistrationTable->setSortingEnabled(false);
//   for (const std::shared_ptr<autoxtime::proto::EventRegistration>& p_er : eventRegistrations)
//   {
//     // do we know about this driver yet or not?
//     std::int64_t id = p_er->event_registration_id();
//     mEventRegistrations[id] = p_er;
//     updateEventRegistration(p_er);
//   }
//   mpEventRegistrationTable->resizeColumnsToContents();
//   mpEventRegistrationTable->setSortingEnabled(true);

//   // TODO add all of the event IDs, driver ids, etc so we can match them later
//   std::string bind_event_id = ":event_id";
//   std::unordered_map<QString, QVariant> bindings;
//   bindings[bind_event_id.data()] = QVariant::fromValue(mEventId);

//   // find all drivers registered for this event
//   {
//     std::string custom =
//         "JOIN " + EventRegistrationModel::TABLE
//         + " ON " + DriverModel::TABLE + "." + DriverModel::PRIMARY_KEY
//         + " = " + EventRegistrationModel::TABLE + "." + DriverModel::PRIMARY_KEY
//         + " WHERE " + EventRegistrationModel::TABLE + "." + EventModel::PRIMARY_KEY
//         + " = " + bind_event_id;
//     mpDriverModel->findCustomAsync(QString::fromStdString(custom), bindings);
//   }

//   // find all cars registered for this event
//   {
//     std::string custom =
//         "JOIN " + EventRegistrationModel::TABLE
//         + " ON " + CarModel::TABLE + "." + CarModel::PRIMARY_KEY
//         + " = " + EventRegistrationModel::TABLE + "." + CarModel::PRIMARY_KEY
//         + " WHERE " + EventRegistrationModel::TABLE + "." + EventModel::PRIMARY_KEY
//         + " = " + bind_event_id;
//     mpCarModel->findCustomAsync(QString::fromStdString(custom), bindings);
//   }

//   // find all car classes in this event
//   {
//     std::string custom =
//         "WHERE " + CarClassModel::TABLE + "." + CarClassModel::PRIMARY_KEY + " IN ( "
//         + " SELECT DISTINCT " + CarModel::TABLE + "." + CarClassModel::PRIMARY_KEY
//         + " FROM " + EventRegistrationModel::TABLE
//         + " JOIN " + CarModel::TABLE
//         + " ON " + CarModel::TABLE + "." + CarModel::PRIMARY_KEY
//         + " = " + EventRegistrationModel::TABLE + "." + CarModel::PRIMARY_KEY
//         + " WHERE " + EventRegistrationModel::TABLE + "." + EventModel::PRIMARY_KEY
//         + " = " + bind_event_id + " )";
//     mpCarClassModel->findCustomAsync(QString::fromStdString(custom), bindings);
//   }
// }

// void RegistrationTableWidget
// ::setDrivers(const std::vector<std::shared_ptr<autoxtime::proto::Driver>>& drivers)
// {
//   mDrivers.clear();
//   AXT_DEBUG << "Found " << drivers.size() << " drivers for event id: "
//             << mEventId;

//   // _MUST_ stop sorting before messing with data, otherwise the "row" value
//   // could change as we mess with the table items (annoying, but it's OK)
//   mpEventRegistrationTable->setSortingEnabled(false);

//   for (const std::shared_ptr<autoxtime::proto::Driver>& p_driver : drivers)
//   {
//     // do we know about this driver yet or not?
//     std::int64_t id = p_driver->driver_id();
//     mDrivers[id] = p_driver;
//     if (!updateDriver(p_driver))
//     {
//       AXT_ERROR << "A Driver came back without a corresponding event registration: " << id;
//     }
//   }
//   mpEventRegistrationTable->resizeColumnsToContents();
//   mpEventRegistrationTable->setSortingEnabled(true);
// }

// bool RegistrationTableWidget
// ::updateCar(const std::shared_ptr<autoxtime::proto::Car>& pCar)
// {
//   // do we know about this yet or not?
//   std::int64_t id = pCar->car_id();
//   std::unordered_map<std::int64_t, QStandardItem*>::iterator iter =
//       mCarItems.find(id);

//   if (iter != mCarItems.end())
//   {
//     // AXT_DEBUG << "car id exists in the map: " << id;
//     // convert the item into a row index using the table's mapping
//     QModelIndex idx = mpEventItemModel->indexFromItem(iter->second);
//     int row_idx = idx.row();
//     updateItem(row_idx, TABLE_COLUMN_CAR_MODEL,
//                QString::fromStdString(pCar->model()));
//     updateItem(row_idx, TABLE_COLUMN_CAR_NUM,
//                QString("%1").arg(pCar->car_number()));

//     // car color is special because we have to mess with the brush
//     QStandardItem* p_item = mpEventItemModel->item(row_idx, TABLE_COLUMN_CAR_COLOR);
//     if (p_item)
//     {
//       QString color_name = QString::fromStdString(pCar->color());
//       QColor color = autoxtime::util::ColorUtil::nameToColor(color_name);
//       if (color.isValid())
//       {
//         p_item->setBackground(QBrush(color));
//         p_item->setText(color_name);
//       }
//       else
//       {
//         // unknown color
//         p_item->setText("??? " + color_name);
//       }
//     }

//     // add data to the first name column about the class id
//     updateItem(row_idx, TABLE_COLUMN_FIRST_NAME,
//                QString(),
//                TABLE_ROLE_CAR_CLASS_ID,
//                QVariant::fromValue(pCar->car_class_id()));
//     return true;
//   }
//   return false;
// }

// bool RegistrationTableWidget
// ::updateCarClass(const std::shared_ptr<autoxtime::proto::CarClass>& carClass)
// {
//   return false;
// }

// bool RegistrationTableWidget
// ::updateDriver(const std::shared_ptr<autoxtime::proto::Driver>& pDriver)
// {
//   std::int64_t id = pDriver->driver_id();
//   std::unordered_map<std::int64_t, QStandardItem*>::iterator iter =
//       mDriverItems.find(id);

//   if (iter != mDriverItems.end())
//   {
//     // AXT_DEBUG << "driver id exists in the map: " << id;
//     // convert the item into a row index using the table's mapping
//     QModelIndex idx = mpEventItemModel->indexFromItem(iter->second);
//     int row_idx = idx.row();

//     // update existing items with data
//     // add this items id to the FIST_NAME column, by convention
//     updateItem(row_idx, TABLE_COLUMN_FIRST_NAME,
//                QString::fromStdString(pDriver->first_name()));
//     updateItem(row_idx, TABLE_COLUMN_LAST_NAME,
//                QString::fromStdString(pDriver->last_name()));
//     return true;
//   }
//   return false;
// }

// bool RegistrationTableWidget
// ::updateEventRegistration(const std::shared_ptr<autoxtime::proto::EventRegistration>& pEventRegistration)
// {
//   // get current event ID and use it for binding in SQL queries
//   if (pEventRegistration->event_id() != mEventId)
//   {
//     return false;
//   }

//   std::int64_t id = pEventRegistration->event_registration_id();
//   std::unordered_map<std::int64_t, QStandardItem*>::iterator iter =
//       mEventRegistrationItems.find(id);

//   int row_idx = -1;
//   if (iter != mEventRegistrationItems.end())
//   {
//     AXT_DEBUG << "event registration id exists in the map: " << id;
//     // convert the item into a row index using the table's mapping
//     QModelIndex idx = mpEventItemModel->indexFromItem(iter->second);
//     row_idx = idx.row();
//   }
//   else
//   {
//     AXT_DEBUG << "event registration id doesn't exists in the map: " << id;
//     // add a new row at the end of the table
//     row_idx = mpEventItemModel->rowCount();
//     mpEventItemModel->insertRow(row_idx);

//     // add all of the item columns
//     mpEventItemModel->setItem(row_idx, TABLE_COLUMN_FIRST_NAME,
//                               new StandardItem(TABLE_ROLE_EDIT));
//     mpEventItemModel->setItem(row_idx, TABLE_COLUMN_LAST_NAME,
//                               new StandardItem(TABLE_ROLE_EDIT));
//     mpEventItemModel->setItem(row_idx, TABLE_COLUMN_CLASS,
//                               new StandardItem(TABLE_ROLE_EDIT));
//     mpEventItemModel->setItem(row_idx, TABLE_COLUMN_CAR_NUM,
//                               new StandardItem(TABLE_ROLE_EDIT));
//     mpEventItemModel->setItem(row_idx, TABLE_COLUMN_CAR_COLOR,
//                               new StandardItem(TABLE_ROLE_EDIT));
//     mpEventItemModel->setItem(row_idx, TABLE_COLUMN_CAR_MODEL,
//                               new StandardItem(TABLE_ROLE_EDIT));
//     mpEventItemModel->setItem(row_idx, TABLE_COLUMN_CHECKED_IN,
//                               new StandardItem(TABLE_ROLE_EDIT));
//   }

//   // keep track of FIRST_NAME column, by convention
//   QStandardItem* p_item =
//       mpEventItemModel->item(row_idx, TABLE_COLUMN_FIRST_NAME);

//   // keep IDs in the item of the FIRST_NAME column
//   p_item->setData(QVariant::fromValue(pEventRegistration->event_registration_id()),
//                   TABLE_ROLE_EVENT_REGISTRATION_ID);
//   p_item->setData(QVariant::fromValue(pEventRegistration->driver_id()),
//                   TABLE_ROLE_DRIVER_ID);
//   p_item->setData(QVariant::fromValue(pEventRegistration->car_id()),
//                   TABLE_ROLE_CAR_ID);

//   // keep track of mappings from ID -> item
//   mDriverItems[pEventRegistration->driver_id()] = p_item;
//   mEventRegistrationItems[pEventRegistration->event_registration_id()] = p_item;
//   mCarItems[pEventRegistration->car_id()] = p_item;

//   // set data from registration object
//   p_item = mpEventItemModel->item(row_idx, TABLE_COLUMN_CHECKED_IN);

//   Qt::CheckState old_check_state = p_item->checkState();
//   Qt::CheckState check_state =
//       pEventRegistration->checked_in() ? Qt::Checked : Qt::Unchecked;
//   p_item->setCheckState(check_state);
//   p_item->setData(QVariant::fromValue(pEventRegistration->checked_in()),
//                   Qt::DisplayRole);
//   p_item->setData(QVariant::fromValue(check_state),
//                   Qt::CheckStateRole);
//   // only modify checked in entries when checked state changes
//   if (old_check_state != check_state)
//   {
//     if (pEventRegistration->checked_in())
//     {
//       ++mNumCheckedInEntries;
//     }
//     else
//     {
//       --mNumCheckedInEntries;
//     }
//   }
//   emit numEntriesChanged(mEventRegistrations.size(),
//                          mNumCheckedInEntries);
//   return true;
// }

// void RegistrationTableWidget::updateItem(int row,
//                                          int col,
//                                          QString text,
//                                          int dataRole,
//                                          QVariant data)
// {
//   QStandardItem* p_item = mpEventItemModel->item(row, col);
//   if (p_item)
//   {
//     if (!text.isNull())
//     {
//       p_item->setText(text);
//     }
//     if (!data.isNull())
//     {
//       p_item->setData(data, dataRole);
//     }
//   }
//   else
//   {
//     AXT_ERROR << "updateItem() - table cell doesn't exist: "
//               << " row = " << row
//               << " col = " << col;
//   }
// }

// void RegistrationTableWidget
// ::setCars(const std::vector<std::shared_ptr<autoxtime::proto::Car>>& cars)
// {
//   mCars.clear();
//   AXT_DEBUG << "Found " << cars.size() << " cars for event id: "
//             << mEventId;

//   // _MUST_ stop sorting before messing with data, otherwise the "row" value
//   // could change as we mess with the table items (annoying, but it's OK)
//   mpEventRegistrationTable->setSortingEnabled(false);

//   for (const std::shared_ptr<autoxtime::proto::Car>& p_car : cars)
//   {
//     std::int64_t id = p_car->car_id();
//     mCars[id] = p_car;
//     if (!updateCar(p_car))
//     {
//       AXT_ERROR << "A car came back without a corresponding event registration: " << id;
//     }
//   }
//   mpEventRegistrationTable->resizeColumnsToContents();
//   mpEventRegistrationTable->setSortingEnabled(true);

//   // update car classes in case they came back before the cars did
//   updateCarClasses();
// }

// void RegistrationTableWidget
// ::setCarClasses(const std::vector<std::shared_ptr<autoxtime::proto::CarClass>>& carClasses)
// {
//   AXT_DEBUG << "Found " << carClasses.size() << " cars classes for event id: "
//             << mEventId;
//   mCarClasses.clear();
//   for (const std::shared_ptr<autoxtime::proto::CarClass>& p_cc : carClasses)
//   {
//     mCarClasses[p_cc->car_class_id()] = p_cc;
//   }

//   // update car classes in case they came back after the cars did
//   updateCarClasses();
// }

// void RegistrationTableWidget::updateCarClasses()
// {
//   AXT_DEBUG << "updateCarClasses - begin";
//   if (mCars.empty() || mCarItems.empty() || mCarClasses.empty())
//   {
//     AXT_DEBUG << "updateCarClasses() - cars or classes are empty";
//     return;
//   }
//   AXT_DEBUG << "updateCarClasses() - cars size = " << mCarItems.size()
//             << " car classes size = " << mCarClasses.size();

//   mpEventRegistrationTable->setSortingEnabled(false);
//   for (std::pair<const std::int64_t, QStandardItem*>& id_item : mCarItems)
//   {
//     QStandardItem* p_item = id_item.second;
//     QVariant data = p_item->data(TABLE_ROLE_CAR_CLASS_ID);
//     if (data.isNull())
//     {
//       AXT_DEBUG << "upcateCarClasses() - Data is null";
//       continue;
//     }

//     // lookup the car class by ID to get its name
//     std::int64_t car_class_id = data.toLongLong();
//     std::unordered_map<std::int64_t, std::shared_ptr<autoxtime::proto::CarClass>>::iterator iter =
//         mCarClasses.find(car_class_id);
//     if (iter == mCarClasses.end())
//     {
//       AXT_DEBUG << "updateCarClasses() - couldn't find car class with id: " << car_class_id;
//       continue;
//     }

//     // get the name of the car class
//     std::shared_ptr<autoxtime::proto::CarClass>& p_cc = iter->second;
//     std::string name = p_cc->name();
//     if (p_cc->has_subclass_name() && !p_cc->subclass_name().empty())
//     {
//       name += "-" + p_cc->subclass_name();
//     }

//     // add text to the item
//     QModelIndex idx = mpEventItemModel->indexFromItem(p_item);
//     int row_idx = idx.row();
//     updateItem(row_idx, TABLE_COLUMN_CLASS,
//                QString::fromStdString(name));
//   }
//   mpEventRegistrationTable->resizeColumnsToContents();
//   mpEventRegistrationTable->setSortingEnabled(true);
//   AXT_DEBUG << "updateCarClasses - done";
// }

// void RegistrationTableWidget::carNotification(const std::shared_ptr<google::protobuf::Message>& pMessage,
//                                               const QDateTime& timestamp,
//                                               const QString& operation)
// {
//   AXT_DEBUG << "RegistrationTableWidget - car notification";
//   std::shared_ptr<autoxtime::proto::Car> p_car =
//       CarModel::messageToT(pMessage);
//   // only update if we care about it
//   if (mCars.find(p_car->car_id()) != mCars.end())
//   {
//     updateCar(p_car);
//   }
// }

// void RegistrationTableWidget::carClassNotification(const std::shared_ptr<google::protobuf::Message>& pMessage,
//                                                    const QDateTime& timestamp,
//                                                    const QString& operation)
// {
//   AXT_DEBUG << "RegistrationTableWidget - car class notification";
// }

// void RegistrationTableWidget::driverNotification(const std::shared_ptr<google::protobuf::Message>& pMessage,
//                                                  const QDateTime& timestamp,
//                                                  const QString& operation)
// {
//   AXT_DEBUG << "RegistrationTableWidget - driver notification " << operation << "\n"
//             << pMessage->DebugString();
//   std::shared_ptr<autoxtime::proto::Driver> p_driver =
//       DriverModel::messageToT(pMessage);
//   // only update if we care about it
//   if (mDrivers.find(p_driver->driver_id()) != mDrivers.end())
//   {
//     updateDriver(p_driver);
//   }
// }

// void RegistrationTableWidget::eventRegistrationNotification(const std::shared_ptr<google::protobuf::Message>& pMessage,
//                                                             const QDateTime& timestamp,
//                                                             const QString& operation)
// {
//   AXT_DEBUG << "RegistrationTableWidget - event registration notification";

//   std::shared_ptr<autoxtime::proto::EventRegistration> p_er =
//       EventRegistrationModel::messageToT(pMessage);
//   // only update if we care about it
//   if (mEventRegistrations.find(p_er->event_registration_id()) != mEventRegistrations.end())
//   {
//     updateEventRegistration(p_er);
//   }
// }

// void RegistrationTableWidget::modelDataChanged(const QModelIndex& topLeft,
//                                                const QModelIndex& bottomRight,
//                                                const QVector<int>& roles)
// {
//   if (!topLeft.isValid() || !bottomRight.isValid())
//   {
//     AXT_DEBUG << "modelDataChanged() - index is invalid";
//     return;
//   }

//   // we only care about TABLE_ROLE_EDIT events
//   if (!roles.contains(TABLE_ROLE_EDIT))
//   {
//     return;
//   }

//   AXT_DEBUG << "Got data changes EDIT event"
//             << " roles=" << roles
//             << " tl_row=" << topLeft.row()
//             << " tl_col=" << topLeft.column()
//             << " br_row=" << bottomRight.row()
//             << " br_col=" << bottomRight.column();
//   for (int r = topLeft.row(); r <= bottomRight.row(); ++r)
//   {
//     QModelIndex id_index =
//           mpEventRegistrationTable->model()->index(r, TABLE_COLUMN_FIRST_NAME);

//     std::int64_t car_id = id_index.data(TABLE_ROLE_CAR_ID).toLongLong();
//     // std::int64_t car_class_id = id_index.data(TABLE_ROLE_CAR_CLASS_ID).toLongLong();
//     std::int64_t driver_id = id_index.data(TABLE_ROLE_DRIVER_ID).toLongLong();
//     std::int64_t er_id = id_index.data(TABLE_ROLE_EVENT_REGISTRATION_ID).toLongLong();

//     for (int c = topLeft.column(); c <= bottomRight.column(); ++c)
//     {
//       QModelIndex index =
//           mpEventRegistrationTable->model()->index(r, c);
//       QVariant data = index.data();

//       switch (static_cast<TableColumn>(c))
//       {
//         case TABLE_COLUMN_FIRST_NAME:
//           // driver
//           {
//             std::unordered_map<std::int64_t, std::shared_ptr<autoxtime::proto::Driver>>::iterator iter =
//                 mDrivers.find(driver_id);
//             if (iter != mDrivers.end())
//             {
//               std::shared_ptr<autoxtime::proto::Driver> p_driver = iter->second;
//               p_driver->set_first_name(data.toString().toStdString());
//               mpDriverModel->update(*p_driver);
//             }
//           }
//           break;
//         case TABLE_COLUMN_LAST_NAME:
//           // driver
//           {
//             std::unordered_map<std::int64_t, std::shared_ptr<autoxtime::proto::Driver>>::iterator iter =
//                 mDrivers.find(driver_id);
//             if (iter != mDrivers.end())
//             {
//               std::shared_ptr<autoxtime::proto::Driver> p_driver = iter->second;
//               p_driver->set_last_name(data.toString().toStdString());
//               mpDriverModel->update(*p_driver);
//             }
//           }
//           break;
//         case TABLE_COLUMN_CLASS:
//           // carclass
//           {
//             // TODO find a new car class by name
//           }
//           break;
//         case TABLE_COLUMN_CAR_NUM:
//           // car
//           {
//             std::unordered_map<std::int64_t, std::shared_ptr<autoxtime::proto::Car>>::iterator iter =
//                 mCars.find(car_id);
//             if (iter != mCars.end())
//             {
//               std::shared_ptr<autoxtime::proto::Car> p_car = iter->second;
//               p_car->set_car_number(data.toLongLong());
//               mpCarModel->update(*p_car);
//             }
//           }
//           break;
//         case TABLE_COLUMN_CAR_COLOR:
//           // car
//           {
//             std::unordered_map<std::int64_t, std::shared_ptr<autoxtime::proto::Car>>::iterator iter =
//                 mCars.find(car_id);
//             if (iter != mCars.end())
//             {
//               std::shared_ptr<autoxtime::proto::Car> p_car = iter->second;
//               p_car->set_color(data.toString().toStdString());
//               mpCarModel->update(*p_car);
//             }
//           }
//          break;
//         case TABLE_COLUMN_CAR_MODEL:
//           // car
//           {
//             std::unordered_map<std::int64_t, std::shared_ptr<autoxtime::proto::Car>>::iterator iter =
//                 mCars.find(car_id);
//             if (iter != mCars.end())
//             {
//               std::shared_ptr<autoxtime::proto::Car> p_car = iter->second;
//               p_car->set_model(data.toString().toStdString());
//               mpCarModel->update(*p_car);
//             }
//           }
//           break;
//         case TABLE_COLUMN_CHECKED_IN:
//           // event registration
//           {
//             std::unordered_map<std::int64_t, std::shared_ptr<autoxtime::proto::EventRegistration>>::iterator iter =
//                 mEventRegistrations.find(er_id);
//             if (iter != mEventRegistrations.end())
//             {
//               std::shared_ptr<autoxtime::proto::EventRegistration> p_er = iter->second;
//               AXT_DEBUG << "Setting checked in column data_str="
//                         << data.toString()
//                         << " data_bool="
//                         << data.toBool();
//               p_er->set_checked_in(data.toBool());
//               mpEventRegistrationModel->update(*p_er);
//               if (data.toBool())
//               {
//                 ++mNumCheckedInEntries;
//               }
//               else
//               {
//                 --mNumCheckedInEntries;
//               }
//               emit numEntriesChanged(mEventRegistrations.size(),
//                                      mNumCheckedInEntries);
//             }
//           }
//           break;
//       }
//     }
//   }
// }

// std::uint64_t RegistrationTableWidget::numCheckedInEntries() const
// {
//   return mNumCheckedInEntries;
// }

// AUTOXTIME_UI_NAMESPACE_END
