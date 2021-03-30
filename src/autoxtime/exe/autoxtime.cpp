#include <autoxtime/autoxtime.h>

// autoxtime
#include <autoxtime/exe/AppCommon.h>
#include <autoxtime/log/Log.h>
#include <autoxtime/ui/MainWindow.h>
#include <autoxtime/codec/MotorSportsRegCodec.h>

// Qt
#include <QApplication>

// DEBUGGING, TODO REMOVE
#include <google/protobuf/util/json_util.h>
#include <autoxtime/db/DriverModel.h>
#include <autoxtime/db/CarClassModel.h>
#include <autoxtime/db/CarModel.h>
#include <autoxtime/db/EventRegistrationModel.h>


int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  autoxtime::AppCommon::init(&app, "autoxtime");

  // // create and launch our main window
  // autoxtime::ui::MainWindow w;
  // w.show();
  // return app.exec();

  autoxtime::MotorSportsRegCodec reader;
  std::vector<std::shared_ptr<autoxtime::MotorSportsRegEntry>> results =
      reader.readFile("../data/20201004-AxWareEventExport.csv");
  {
    google::protobuf::util::JsonPrintOptions opts;
    opts.add_whitespace = true;
    for (const std::shared_ptr<autoxtime::MotorSportsRegEntry>& r : results)
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
        return -1;
      }

    }
  }
  return 0;
}
