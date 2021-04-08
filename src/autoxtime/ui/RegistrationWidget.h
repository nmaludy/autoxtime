#ifndef AUTOXTIME_UI_REGISTRATIONWIDGET
#define AUTOXTIME_UI_REGISTRATIONWIDGET

#include <autoxtime/ui/ui.h>
#include <QWidget>

class QComboBox;
class QTableWidget;

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

 private:
  autoxtime::db::CarClassModel* mpCarClassModel;
  autoxtime::db::CarModel* mpCarModel;
  autoxtime::db::DriverModel* mpDriverModel;
  autoxtime::db::EventModel* mpEventModel;
  autoxtime::db::EventRegistrationModel* mpEventRegistrationModel;

  QComboBox* mpEventComboBox;

  // table
  QTableWidget* mpEventRegistrationTable;
};

AUTOXTIME_UI_NAMESPACE_END

#endif // AUTOXTIME_UI_REGISTRATIONWIDGET
