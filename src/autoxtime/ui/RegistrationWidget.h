#ifndef AUTOXTIME_UI_REGISTRATIONWIDGET
#define AUTOXTIME_UI_REGISTRATIONWIDGET

#include <autoxtime/ui/ui.h>
#include <QWidget>
#include <QVariant>

class QComboBox;
class QLineEdit;
class QSortFilterProxyModel;
class QStandardItem;
class QStandardItemModel;
class QTableView;

namespace autoxtime { namespace db { class CarModel; } }
namespace autoxtime { namespace db { class CarClassModel; } }
namespace autoxtime { namespace db { class DriverModel; } }
namespace autoxtime { namespace db { class EventModel; } }
namespace autoxtime { namespace db { class EventRegistrationModel; } }

namespace autoxtime { namespace proto { class Car; } }
namespace autoxtime { namespace proto { class CarClass; } }
namespace autoxtime { namespace proto { class Driver; } }
namespace autoxtime { namespace proto { class Event; } }
namespace autoxtime { namespace proto { class EventRegistration; } }

namespace autoxtime { namespace ui { class MultiSortFilterProxyModel; } }

AUTOXTIME_UI_NAMESPACE_BEG

class RegistrationWidget : public QWidget
{
  Q_OBJECT

 public:
  explicit RegistrationWidget(QWidget* pParent = nullptr);

 public slots:
  void setCarClasses(const std::vector<std::shared_ptr<autoxtime::proto::CarClass>>& carClasses);
  void setCars(const std::vector<std::shared_ptr<autoxtime::proto::Car>>& cars);
  void setDrivers(const std::vector<std::shared_ptr<autoxtime::proto::Driver>>& drivers);
  void setEventRegistrations(const std::vector<std::shared_ptr<autoxtime::proto::EventRegistration>>& eventRegistrations);
  void setEvents(const std::vector<std::shared_ptr<autoxtime::proto::Event>>& events);
  void eventComboIndexChanged(int index);
  void filterChanged(const QString& text);

 private:
  void resetTable();
  void updateItem(int row,
                  int col,
                  QString text,
                  int dataRole = -1,
                  QVariant data = QVariant());
  void updateCarClasses();

  enum TableColumn
  {
    TABLE_COLUMN_FIRST_NAME = 0,
    TABLE_COLUMN_LAST_NAME  = 1,
    TABLE_COLUMN_CLASS      = 2,
    TABLE_COLUMN_CAR_NUM    = 3,
    TABLE_COLUMN_CAR_COLOR  = 4,
    TABLE_COLUMN_CAR        = 5,
    TABLE_COLUMN_CHECKED_IN = 6
  };
  enum TableData
  {
    TABLE_ROLE_CAR_ID = Qt::UserRole,
    TABLE_ROLE_CAR_CLASS_ID,
    TABLE_ROLE_DRIVER_ID,
    TABLE_ROLE_EVENT_REGISTRATION_ID
  };


  // models
  autoxtime::db::CarClassModel* mpCarClassModel;
  autoxtime::db::CarModel* mpCarModel;
  autoxtime::db::DriverModel* mpDriverModel;
  autoxtime::db::EventModel* mpEventModel;
  autoxtime::db::EventRegistrationModel* mpEventRegistrationModel;

  // data
  std::vector<std::shared_ptr<autoxtime::proto::Car>> mCars;
  std::unordered_map<std::int64_t, std::shared_ptr<autoxtime::proto::CarClass>> mCarClasses;
  std::vector<std::shared_ptr<autoxtime::proto::Driver>> mDrivers;
  std::vector<std::shared_ptr<autoxtime::proto::EventRegistration>> mEventRegistrations;

  // mapping id to table items
  // each "cell" in the table is a different item, so by convention, we're going to use
  // the first column ("First Name" aka TABLE_COLUMN_FIRST_NAME) to store the IDs of
  // all the different types.
  // from this item, we can retrieve the current row and then edit the other items in that row
  // as necessary
  std::unordered_map<std::int64_t, QStandardItem*> mCarItems;
  // don't need carclass items because we map those a different way
  // (their ID is meaningless really)
  std::unordered_map<std::int64_t, QStandardItem*> mDriverItems;
  std::unordered_map<std::int64_t, QStandardItem*> mEventRegistrationItems;

  // combo box
  QComboBox* mpEventComboBox;

  // filter
  QLineEdit* mpFilterLineEdit;

  // table
  QTableView* mpEventRegistrationTable;
  QStandardItemModel* mpEventItemModel;
  MultiSortFilterProxyModel* mpEventSortFilterProxyModel;
  std::int64_t mTableRowAddIdx;
};

AUTOXTIME_UI_NAMESPACE_END

#endif // AUTOXTIME_UI_REGISTRATIONWIDGET
