#include <autoxtime/ui/EventImportDialog.h>

// autoxtime
#include <autoxtime/log/Log.h>
#include <autoxtime/codec/MotorsportRegCodec.h>
#include <autoxtime/db/DriverModel.h>
#include <autoxtime/db/CarClassModel.h>
#include <autoxtime/db/CarModel.h>
#include <autoxtime/db/EventRegistrationModel.h>

// protobuf
#include <google/protobuf/util/json_util.h>

// Qt
#include <QFileDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

AUTOXTIME_UI_NAMESPACE_BEG

EventImportDialog::EventImportDialog(std::int64_t eventId,
                                     QWidget* pParent)
    : QDialog(pParent),
      mEventId(eventId),
      mpFormatComboBox(new QComboBox(this)),
      mpFileLineEdit(new QLineEdit(this)),
      mpFileBrowseButton(new QPushButton("Browse", this)),
      mpSaveButton(new QPushButton("Save", this)),
      mpCancelButton(new QPushButton("Cancel", this))
{
  QGridLayout* p_evemt_layout = new QGridLayout(this);

  // ComboBox
  {
    QStringList formats{"MotorsportReg Export (*.csv)"};
    mpFormatComboBox->addItems(formats);
    p_evemt_layout->addWidget(new QLabel("Format", this), 0, 0);
    p_evemt_layout->addWidget(mpFormatComboBox, 0, 1, 1, -1);
  }

  // File selection
  {
    // read-only
    mpFileLineEdit->setEnabled(false);

    connect(mpFileBrowseButton, &QPushButton::clicked,
            this,               &EventImportDialog::browseClicked);

    p_evemt_layout->addWidget(new QLabel("File", this), 1, 0);
    p_evemt_layout->addWidget(mpFileLineEdit, 1, 1, 1, -1);
    p_evemt_layout->addWidget(mpFileBrowseButton, 1, 2);
  }

  // Save/Cancel
  {
    // disable until we load a file
    mpSaveButton->setEnabled(false);
    mpCancelButton->setEnabled(true);

    // capture save/cancel events
    connect(mpSaveButton, &QPushButton::clicked,
            this,         &EventImportDialog::saveClicked);
    connect(mpCancelButton, &QPushButton::clicked,
            this,           &QDialog::reject);

    p_evemt_layout->addWidget(mpSaveButton, 4, 0);
    p_evemt_layout->addWidget(mpCancelButton, 4, 1, 1, -1);
  }

  // stretch last row
  p_evemt_layout->setRowStretch(3, 1);
}

void EventImportDialog::browseClicked(bool checked)
{
  // TODO select file dialog
  AXT_DEBUG << "Browse clicked, format = " << mpFormatComboBox->currentText();
  QString filename =
      QFileDialog::getOpenFileName(this,
                                   QString("Import Event File"),
                                   QString(),
                                   mpFormatComboBox->currentText(),
                                   nullptr, // selected filter
                                   QFileDialog::DontUseNativeDialog);
  if (filename.isEmpty())
  {
    return;
  }

  mpFileLineEdit->setText(filename);
  AXT_DEBUG << "Browse selected file: " << filename;

  // load file
  // TODO choose based on FORMAT
  autoxtime::MotorsportRegCodec reader;
  std::vector<std::shared_ptr<autoxtime::MotorsportRegEntry>> results =
      reader.readFile(filename);

  // TODO display imported data

  /////////////
  // TODO only this on save
  google::protobuf::util::JsonPrintOptions opts;
  opts.add_whitespace = true;
  for (const std::shared_ptr<autoxtime::MotorsportRegEntry>& r : results)
  {
    AXT_DEBUG << "######################################################";
    {
      std::string out;
      google::protobuf::util::MessageToJsonString(*(r->mpDriver.get()), &out, opts);
      AXT_DEBUG << "Driver from csv: " << QString::fromStdString(out);
    }
    {
      std::string out;
      google::protobuf::util::MessageToJsonString(*(r->mpCar.get()), &out, opts);
      AXT_DEBUG << "Car from csv: " << QString::fromStdString(out);
    }
    {
      std::string out;
      google::protobuf::util::MessageToJsonString(*(r->mpCarClass.get()), &out, opts);
      AXT_DEBUG << "Car Class from csv: " << QString::fromStdString(out);
    }
    {
      std::string out;
      google::protobuf::util::MessageToJsonString(*(r->mpEventRegistration.get()), &out, opts);
      AXT_DEBUG << "Event Registration from csv: " << QString::fromStdString(out);
    }

    // order of creation
    // organization (need to look this up)
    int org_id = 1;
    int event_id = 1;
    int season_id = 1;

    int driver_id = -1;
    int car_class_id = -1;
    int car_id = -1;
    int event_registration_id = -1;

    // driver
    {
      autoxtime::db::DriverModel model;
      std::vector<std::shared_ptr<autoxtime::proto::Driver> > created =
          model.createIfNotExists(*(r->mpDriver));
      AXT_DEBUG << "driver created size = " << created.size();
      if (!created.empty())
      {
        driver_id = created[0]->driver_id();
      }
    }

    // car class
    {
      autoxtime::db::CarClassModel model;
      r->mpCarClass->set_organization_id(org_id);
      std::vector<std::shared_ptr<autoxtime::proto::CarClass> > created =
          model.createIfNotExists(*(r->mpCarClass));
      AXT_DEBUG << "car class created size = " << created.size();
      if (!created.empty())
      {
        car_class_id = created[0]->car_class_id();
      }
    }

    // car
    {
      autoxtime::db::CarModel model;
      r->mpCar->set_organization_id(org_id);
      r->mpCar->set_car_class_id(car_class_id);
      r->mpCar->set_driver_id(driver_id);

      std::vector<std::shared_ptr<autoxtime::proto::Car> > created =
          model.createIfNotExists(*(r->mpCar));
      AXT_DEBUG << "car created size = " << created.size();
      if (!created.empty())
      {
        car_id = created[0]->car_id();
      }
    }

    // event registration
    {
      autoxtime::db::EventRegistrationModel model;
      r->mpEventRegistration->set_event_id(event_id);
      r->mpEventRegistration->set_driver_id(driver_id);

      std::vector<std::shared_ptr<autoxtime::proto::EventRegistration> > created =
          model.createIfNotExists(*(r->mpEventRegistration));
      AXT_DEBUG << "event registration created size = " << created.size();
      if (!created.empty())
      {
        event_registration_id = created[0]->event_registration_id();
      }
    }

    AXT_DEBUG << "Created: \n"
              << "  driver_id = " << driver_id << "\n"
              << "  car_class_id = " << car_class_id << "\n"
              << "  car_id = " << car_id << "\n"
              << "  event_registration_id = " << event_registration_id;
    if (driver_id == -1 ||
        car_class_id == -1 ||
        car_id == -1 ||
        event_registration_id == -1)
    {
      AXT_ERROR << "Failed to get or create";
    }
  }
  ///////////
}

void EventImportDialog::saveClicked(bool checked)
{
  // TODO, write to SQL
  AXT_DEBUG << "Save clicked";
  accept();
}

AUTOXTIME_UI_NAMESPACE_END
