#ifndef AUTOXTIME_UI_REGISTRATIONTABLEWIDGET
#define AUTOXTIME_UI_REGISTRATIONTABLEWIDGET

#include <autoxtime/ui/ui.h>
#include <QList>
#include <QWidget>

class QLineEdit;
class QTableView;

namespace autoxtime { namespace ui { class MultiSortFilterProxyModel; } }
namespace autoxtime { namespace ui { class RegistrationTableModel; } }
namespace autoxtime { namespace ui { class RegistrationTableModel; } }

AUTOXTIME_UI_NAMESPACE_BEG

class RegistrationTableWidget : public QWidget
{
  Q_OBJECT

 public:
  explicit RegistrationTableWidget(QWidget* pParent = nullptr);

 signals:
  void numEntriesChanged(std::uint64_t numEntries,
                         std::uint64_t numCheckInEntries);

 public slots:
  void setEventId(std::int64_t eventId);
  void modelNumEntriesChanged(std::uint64_t numEntries,
                              std::uint64_t numCheckInEntries);
  void filterChanged(const QString& text);
  void filterColumnToggled();

 private:

  // filter
  QLineEdit* mpFilterLineEdit;

  // table
  QTableView* mpTableView;
  RegistrationTableModel* mpRegistrationTableModel;
  MultiSortFilterProxyModel* mpSortFilterProxyModel;

  QStringList mTableColumnHeaders;
  QList<int> mTableFilterColumns;
  QList<QAction*> mTableFilterColumnActions;
};

AUTOXTIME_UI_NAMESPACE_END

#endif // AUTOXTIME_UI_REGISTRATIONTABLEWIDGET

// #ifndef AUTOXTIME_UI_REGISTRATIONTABLEWIDGET
// #define AUTOXTIME_UI_REGISTRATIONTABLEWIDGET

// #include <autoxtime/ui/ui.h>
// #include <QWidget>
// #include <QVariant>

// class QAction;
// class QLineEdit;
// class QStandardItem;
// class QStandardItemModel;
// class QTableView;

// namespace autoxtime { namespace db { class CarModel; } }
// namespace autoxtime { namespace db { class CarClassModel; } }
// namespace autoxtime { namespace db { class DriverModel; } }
// namespace autoxtime { namespace db { class EventModel; } }
// namespace autoxtime { namespace db { class EventRegistrationModel; } }

// namespace autoxtime { namespace proto { class Car; } }
// namespace autoxtime { namespace proto { class CarClass; } }
// namespace autoxtime { namespace proto { class Driver; } }
// namespace autoxtime { namespace proto { class EventRegistration; } }

// namespace autoxtime { namespace ui { class MultiSortFilterProxyModel; } }

// namespace google { namespace protobuf { class Message; } }

// AUTOXTIME_UI_NAMESPACE_BEG

// class RegistrationTableWidget : public QWidget
// {
//   Q_OBJECT

//  public:
//   explicit RegistrationTableWidget(QWidget* pParent = nullptr);

//   enum TableColumn
//   {
//     TABLE_COLUMN_FIRST_NAME = 0,
//     TABLE_COLUMN_LAST_NAME,
//     TABLE_COLUMN_CLASS,
//     TABLE_COLUMN_CAR_NUM,
//     TABLE_COLUMN_CAR_COLOR,
//     TABLE_COLUMN_CAR_MODEL,
//     TABLE_COLUMN_CHECKED_IN
//   };
//   enum TableData
//   {
//     TABLE_ROLE_EDIT = Qt::UserRole,
//     TABLE_ROLE_CAR_CLASS_ID,
//     TABLE_ROLE_DRIVER_ID,
//     TABLE_ROLE_EVENT_REGISTRATION_ID,
//     TABLE_ROLE_CAR_ID
//   };

//   std::uint64_t numCheckedInEntries() const;

//  signals:
//   void numEntriesChanged(std::uint64_t numEntries,
//                          std::uint64_t numCheckInEntries);

//  public slots:
//   void setEventId(std::int64_t eventId);

//   void setCarClasses(const std::vector<std::shared_ptr<autoxtime::proto::CarClass>>& carClasses);
//   void setCars(const std::vector<std::shared_ptr<autoxtime::proto::Car>>& cars);
//   void setDrivers(const std::vector<std::shared_ptr<autoxtime::proto::Driver>>& drivers);
//   void setEventRegistrations(const std::vector<std::shared_ptr<autoxtime::proto::EventRegistration>>& eventRegistrations);
//   void filterChanged(const QString& text);
//   void filterColumnToggled();

//   void carNotification(const std::shared_ptr<google::protobuf::Message>& pMessage,
//                        const QDateTime& timestamp,
//                        const QString& operation);
//   void carClassNotification(const std::shared_ptr<google::protobuf::Message>& pMessage,
//                             const QDateTime& timestamp,
//                             const QString& operation);
//   void driverNotification(const std::shared_ptr<google::protobuf::Message>& pMessage,
//                           const QDateTime& timestamp,
//                           const QString& operation);
//   void eventRegistrationNotification(const std::shared_ptr<google::protobuf::Message>& pMessage,
//                                      const QDateTime& timestamp,
//                                      const QString& operation);

//   void modelDataChanged(const QModelIndex& topLeft,
//                         const QModelIndex& bottomRight,
//                         const QVector<int>& roles);

//  private:
//   void resetTable();

//   bool updateCar(const std::shared_ptr<autoxtime::proto::Car>& car);
//   bool updateCarClass(const std::shared_ptr<autoxtime::proto::CarClass>& carClass);
//   bool updateDriver(const std::shared_ptr<autoxtime::proto::Driver>& driver);
//   bool updateEventRegistration(const std::shared_ptr<autoxtime::proto::EventRegistration>& eventRegistration);
//   void updateItem(int row,
//                   int col,
//                   QString text,
//                   int dataRole = -1,
//                   QVariant data = QVariant());
//   void updateCarClasses();

//   // models
//   autoxtime::db::CarClassModel* mpCarClassModel;
//   autoxtime::db::CarModel* mpCarModel;
//   autoxtime::db::DriverModel* mpDriverModel;
//   autoxtime::db::EventRegistrationModel* mpEventRegistrationModel;

//   // data
//   std::unordered_map<std::int64_t, std::shared_ptr<autoxtime::proto::Car>> mCars;
//   std::unordered_map<std::int64_t, std::shared_ptr<autoxtime::proto::CarClass>> mCarClasses;
//   std::unordered_map<std::int64_t, std::shared_ptr<autoxtime::proto::Driver>> mDrivers;
//   std::unordered_map<std::int64_t, std::shared_ptr<autoxtime::proto::EventRegistration>> mEventRegistrations;

//   // mapping id to table items
//   // each "cell" in the table is a different item, so by convention, we're going to use
//   // the first column ("First Name" aka TABLE_COLUMN_FIRST_NAME) to store the IDs of
//   // all the different types.
//   // from this item, we can retrieve the current row and then edit the other items in that row
//   // as necessary
//   std::unordered_map<std::int64_t, QStandardItem*> mCarItems;
//   // don't need carclass items because we map those a different way
//   // (their ID is meaningless really)
//   std::unordered_map<std::int64_t, QStandardItem*> mDriverItems;
//   std::unordered_map<std::int64_t, QStandardItem*> mEventRegistrationItems;

//   // ID of the event to be shown in the table
//   std::int64_t mEventId;

//   // filter
//   QLineEdit* mpFilterLineEdit;

//   // table
//   QStringList mTableColumnHeaders;
//   QList<int> mTableFilterColumns;
//   QList<QAction*> mTableFilterColumnActions;
//   QTableView* mpEventRegistrationTable;
//   QStandardItemModel* mpEventItemModel;
//   MultiSortFilterProxyModel* mpEventSortFilterProxyModel;
//   std::uint64_t mTableRowAddIdx;

//   // status
//   std::uint64_t mNumCheckedInEntries;
// };

// AUTOXTIME_UI_NAMESPACE_END

// #endif // AUTOXTIME_UI_REGISTRATIONTABLEWIDGET
