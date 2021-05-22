#ifndef AUTOXTIME_UI_REGISTRATIONTABLEMODEL
#define AUTOXTIME_UI_REGISTRATIONTABLEMODEL

#include <autoxtime/ui/ui.h>

// std
#include <memory>

// qt
#include <QColor>
#include <QVariant>
#include <QAbstractTableModel>

namespace autoxtime { namespace db { class CarModel; } }
namespace autoxtime { namespace db { class CarClassModel; } }
namespace autoxtime { namespace db { class DriverModel; } }
namespace autoxtime { namespace db { class EventModel; } }
namespace autoxtime { namespace db { class EventRegistrationModel; } }

namespace autoxtime { namespace proto { class Car; } }
namespace autoxtime { namespace proto { class CarClass; } }
namespace autoxtime { namespace proto { class Driver; } }
namespace autoxtime { namespace proto { class EventRegistration; } }

namespace google { namespace protobuf { class Message; } }

AUTOXTIME_UI_NAMESPACE_BEG

class RegistrationTableModel : public QAbstractTableModel
{
  Q_OBJECT

 public:
  explicit RegistrationTableModel(QWidget* pParent = nullptr);

  enum TableColumn
  {
    TABLE_COLUMN_FIRST_NAME = 0,
    TABLE_COLUMN_LAST_NAME,
    TABLE_COLUMN_CLASS,
    TABLE_COLUMN_CAR_NUM,
    TABLE_COLUMN_CAR_COLOR,
    TABLE_COLUMN_CAR_MODEL,
    TABLE_COLUMN_CHECKED_IN
  };
  enum TableData
  {
    TABLE_ROLE_EDIT = Qt::UserRole,
    TABLE_ROLE_CAR_CLASS_ID,
    TABLE_ROLE_DRIVER_ID,
    TABLE_ROLE_EVENT_REGISTRATION_ID,
    TABLE_ROLE_CAR_ID
  };

  inline std::uint64_t numCheckedInEntries() const;

  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  virtual QVariant	headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
  virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

 signals:
  void numEntriesChanged(std::uint64_t numEntries,
                         std::uint64_t numCheckInEntries);

 public slots:
  void setEventId(std::int64_t eventId);
  inline std::int64_t eventId() const;

  void setCarClasses(const std::vector<std::shared_ptr<autoxtime::proto::CarClass>>& carClasses);
  void setCars(const std::vector<std::shared_ptr<autoxtime::proto::Car>>& cars);
  void setDrivers(const std::vector<std::shared_ptr<autoxtime::proto::Driver>>& drivers);
  void setEventRegistrations(const std::vector<std::shared_ptr<autoxtime::proto::EventRegistration>>& eventRegistrations);

  void carNotification(const std::shared_ptr<google::protobuf::Message>& pMessage,
                       const QDateTime& timestamp,
                       const QString& operation);
  void carClassNotification(const std::shared_ptr<google::protobuf::Message>& pMessage,
                            const QDateTime& timestamp,
                            const QString& operation);
  void driverNotification(const std::shared_ptr<google::protobuf::Message>& pMessage,
                          const QDateTime& timestamp,
                          const QString& operation);
  void eventRegistrationNotification(const std::shared_ptr<google::protobuf::Message>& pMessage,
                                     const QDateTime& timestamp,
                                     const QString& operation);

 private:
  void reset();

  bool updateCar(const std::shared_ptr<autoxtime::proto::Car>& car);
  bool updateCarClass(const std::shared_ptr<autoxtime::proto::CarClass>& carClass);
  bool updateDriver(const std::shared_ptr<autoxtime::proto::Driver>& driver);
  bool updateEventRegistration(const std::shared_ptr<autoxtime::proto::EventRegistration>& eventRegistration);
  void updateCarClasses();

  static const std::unordered_map<TableColumn, QString> COLUMN_HEADER_MAP;

  // models
  autoxtime::db::CarClassModel* mpCarClassModel;
  autoxtime::db::CarModel* mpCarModel;
  autoxtime::db::DriverModel* mpDriverModel;
  autoxtime::db::EventRegistrationModel* mpEventRegistrationModel;

  // data
  struct DataItem
  {
    int row;
    std::shared_ptr<autoxtime::proto::Car> mpCar;
    std::shared_ptr<autoxtime::proto::CarClass> mpCarClass;
    std::shared_ptr<autoxtime::proto::Driver> mpDriver;
    std::shared_ptr<autoxtime::proto::EventRegistration> mpEventRegistration;
    QColor mCarColor;
  };

  std::shared_ptr<DataItem> itemFromIndex(const QModelIndex& index) const;
  void itemDataChanged(const std::shared_ptr<DataItem>& pItem);

  std::unordered_map<std::int64_t, std::shared_ptr<DataItem>> mCars;
  std::unordered_map<std::int64_t, std::shared_ptr<autoxtime::proto::CarClass>> mCarClasses;
  std::unordered_map<std::int64_t, std::shared_ptr<DataItem>> mDrivers;
  std::unordered_map<std::int64_t, std::shared_ptr<DataItem>> mEventRegistrations;
  std::vector<std::shared_ptr<DataItem>> mDataItems;

  // ID of the event to be shown in the table
  std::int64_t mEventId;

  // table
  QStringList mTableColumnHeaders;

  // status
  std::uint64_t mNumCheckedInEntries;
};

inline std::int64_t RegistrationTableModel::eventId() const
{
  return mEventId;
}

inline std::uint64_t RegistrationTableModel::numCheckedInEntries() const
{
  return mNumCheckedInEntries;
}

AUTOXTIME_UI_NAMESPACE_END

#endif // AUTOXTIME_UI_REGISTRATIONTABLEMODEL
