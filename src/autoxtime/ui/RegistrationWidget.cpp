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

#include <QGridLayout>
#include <QLabel>
#include <QComboBox>

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
      mpEventComboBox(new QComboBox(this))
{
  QGridLayout* p_layout = new QGridLayout(this);
  p_layout->addWidget(new QLabel("This will be some registration stuff"), 0, 0);

  // events combo box
  {
    // might want to change this later to not be alphabetical
    mpEventComboBox->setInsertPolicy(QComboBox::InsertAlphabetically);

    connect(mpEventComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,            &RegistrationWidget::eventComboIndexChanged);
    connect(mpEventModel, &autoxtime::db::EventModel::listResult,
            this,         &RegistrationWidget::setEvents);
    mpEventModel->listAsync();
  }

  connect(mpDriverModel, &autoxtime::db::DriverModel::findResult,
          this,          &RegistrationWidget::setDrivers);
  connect(mpEventRegistrationModel, &autoxtime::db::EventRegistrationModel::findResult,
          this,                     &RegistrationWidget::setEventRegistrations);
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
  std::int64_t event_id = mpEventComboBox->currentData().toLongLong();
  autoxtime::proto::EventRegistration er;
  er.set_event_id(event_id);
  // TODO make the find queries async
  mpEventRegistrationModel->findAsync(er);
  // mpEventRegistrationModel->listAsync();
}

void RegistrationWidget
::setEventRegistrations(const std::vector<std::shared_ptr<autoxtime::proto::EventRegistration>>& eventRegistrations)
{
  std::int64_t event_id = mpEventComboBox->currentData().toLongLong();
  AXT_DEBUG << "Found " << eventRegistrations.size() << " registrations for event id: "
            << event_id;

  std::string bind_event_id = ":event_id";
  std::string custom =
      "JOIN " + EventRegistrationModel::TABLE
      + " ON " + DriverModel::TABLE + "." + DriverModel::PRIMARY_KEY
      + " = " + EventRegistrationModel::TABLE + "." + DriverModel::PRIMARY_KEY
      + " WHERE " + EventRegistrationModel::TABLE + "." + EventModel::PRIMARY_KEY
      + " = " + bind_event_id;
  std::unordered_map<QString, QVariant> bindings;
  bindings[bind_event_id.data()] = QVariant::fromValue(event_id);

  mpDriverModel->findCustomAsync(QString::fromStdString(custom), bindings);
}

void RegistrationWidget
::setDrivers(const std::vector<std::shared_ptr<autoxtime::proto::Driver>>& drivers)
{
  AXT_DEBUG << "Found " << drivers.size() << " drivers for this event";
}

void RegistrationWidget
::setCars(const std::vector<std::shared_ptr<autoxtime::proto::Car>>& cars)
{
}

void RegistrationWidget
::setCarClasses(const std::vector<std::shared_ptr<autoxtime::proto::CarClass>>& carClasses)
{
}


AUTOXTIME_UI_NAMESPACE_END
