#ifndef AUTOXTIME_UI_RUNTABLEMODEL
#define AUTOXTIME_UI_RUNTABLEMODEL

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
namespace autoxtime { namespace db { class RunModel; } }

namespace autoxtime { namespace proto { class Car; } }
namespace autoxtime { namespace proto { class CarClass; } }
namespace autoxtime { namespace proto { class Driver; } }
namespace autoxtime { namespace proto { class Run; } }

namespace google { namespace protobuf { class Message; } }

AUTOXTIME_UI_NAMESPACE_BEG

class RunTableModel : public QAbstractTableModel
{
  Q_OBJECT

 public:
  explicit RunTableModel(QWidget* pParent = nullptr);

  enum TableColumn
  {
    TABLE_COLUMN_RUN_NUMBER = 0,
    TABLE_COLUMN_CLASS,
    TABLE_COLUMN_CAR_NUM,
    TABLE_COLUMN_DRIVER_NAME,
    TABLE_COLUMN_CAR_MODEL,
    TABLE_COLUMN_TIME_SECTOR_1,
    TABLE_COLUMN_TIME_SECTOR_2,
    TABLE_COLUMN_TIME_TOTAL,
    TABLE_COLUMN_DNF,
    TABLE_COLUMN_PENALTIES
  };
  static const std::unordered_map<TableColumn, QString> COLUMN_HEADER_MAP;

  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  virtual QVariant	headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
  virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

  void reset();

  std::int64_t indexRunId(const QModelIndex& index) const;

 public slots:
  void setEventId(std::int64_t eventId);
  inline std::int64_t eventId() const;

  void setCarClasses(const std::vector<std::shared_ptr<autoxtime::proto::CarClass>>& carClasses);
  void setCars(const std::vector<std::shared_ptr<autoxtime::proto::Car>>& cars);
  void setDrivers(const std::vector<std::shared_ptr<autoxtime::proto::Driver>>& drivers);
  void setRuns(const std::vector<std::shared_ptr<autoxtime::proto::Run>>& runs);

  void carNotification(const std::shared_ptr<google::protobuf::Message>& pMessage,
                       const QDateTime& timestamp,
                       const QString& operation);
  void carClassNotification(const std::shared_ptr<google::protobuf::Message>& pMessage,
                            const QDateTime& timestamp,
                            const QString& operation);
  void driverNotification(const std::shared_ptr<google::protobuf::Message>& pMessage,
                          const QDateTime& timestamp,
                          const QString& operation);
  void runNotification(const std::shared_ptr<google::protobuf::Message>& pMessage,
                       const QDateTime& timestamp,
                       const QString& operation);

 private:

  bool updateCar(const std::shared_ptr<autoxtime::proto::Car>& car);
  bool updateCarClass(const std::shared_ptr<autoxtime::proto::CarClass>& carClass);
  bool updateDriver(const std::shared_ptr<autoxtime::proto::Driver>& driver);
  bool updateRun(const std::shared_ptr<autoxtime::proto::Run>& run);
  void updateCarClasses();
  void endResetIfPopulated();

  // models
  autoxtime::db::CarClassModel* mpCarClassModel;
  autoxtime::db::CarModel* mpCarModel;
  autoxtime::db::DriverModel* mpDriverModel;
  autoxtime::db::RunModel* mpRunModel;

  // data
  struct DataItem
  {
    explicit DataItem();
    int row;
    std::shared_ptr<autoxtime::proto::Car> mpCar;
    std::shared_ptr<autoxtime::proto::CarClass> mpCarClass;
    std::shared_ptr<autoxtime::proto::Driver> mpDriver;
    std::shared_ptr<autoxtime::proto::Run> mpRun;
    QColor mCarColor;
  };

  std::shared_ptr<DataItem> itemFromIndex(const QModelIndex& index) const;
  void itemDataChanged(const std::shared_ptr<DataItem>& pItem);

  std::unordered_map<std::int64_t, std::shared_ptr<DataItem>> mCars;
  std::unordered_map<std::int64_t, std::shared_ptr<autoxtime::proto::CarClass>> mCarClasses;
  std::unordered_map<std::int64_t, std::shared_ptr<DataItem>> mDrivers;
  std::unordered_map<std::int64_t, std::shared_ptr<DataItem>> mRuns;
  std::vector<std::shared_ptr<DataItem>> mDataItems;

  bool mCarsPopulated;
  bool mCarClassesPopulated;
  bool mDriversPopulated;
  bool mRunsPopulated;

  // ID of the event to be shown in the table
  std::int64_t mEventId;
};

inline std::int64_t RunTableModel::eventId() const
{
  return mEventId;
}

AUTOXTIME_UI_NAMESPACE_END

#endif // AUTOXTIME_UI_RUNTABLEMODEL
