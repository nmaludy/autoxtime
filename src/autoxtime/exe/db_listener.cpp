#include <autoxtime/autoxtime.h>
#include <autoxtime/config/ConfigStore.h>
#include <autoxtime/log/Log.h>
#include <autoxtime/db/DbListener.h>
#include <autoxtime/db/DriverModel.h>
#include <autoxtime/proto/driver.pb.h>

// Qt
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QHash>
#include <QSerialPortInfo>

#include <google/protobuf/util/json_util.h>
#include <QtDebug>

void initCliParser(QCommandLineParser& parser)
{
  parser.setApplicationDescription("Reads data from timers (serial) and sends it to the server");
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addOption(QCommandLineOption(QStringList() << "c" << "config",
                                      "Path to the config file",
                                      "config",
                                      "./config/db.conf"));
}

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName(AUTOXTIME_ORG_NAME);
  QCoreApplication::setOrganizationDomain(AUTOXTIME_ORG_DOMAIN);
  QCoreApplication::setApplicationName("autoxtime_db_listener");
  QCoreApplication::setApplicationVersion(AUTOXTIME_VERSION_STR);

  // Process the actual command line arguments given by the user
  QCommandLineParser parser;
  initCliParser(parser);
  parser.process(app);

  // initialize our config with our parser and our map before initializing any other
  // classes, this way other classes can just call ConfigStore::instance().value()
  // and get what they are after
  autoxtime::ConfigStore::init(parser.value("config"), &parser);

  // autoxtime::db::DbListener db("driver");

  autoxtime::db::DriverModel driver;

  // create tests
  // {
  //   autoxtime::proto::Driver d;
  //   d.set_first_name("Nick3");
  //   d.set_last_name("Maludy");
  //   d.set_email("nick@test.com");
  //   d.set_phone_number("555-123-4567");
  //   d.set_msr_id(2);
  //   d.set_scca_id(2);
  //   driver.create(d);
  // }
  // delete tests
  // AXT_DEBUG << "num drivers deleted: " << driver.destroyById(8);

  // update test
  // {
  //   autoxtime::proto::Driver d;
  //   d.set_driver_id(9);
  //   d.set_first_name("junk");
  //   d.set_last_name("44dsfds");
  //   d.set_email("nick@test.com");
  //   d.set_phone_number("555-123-4567");
  //   d.set_msr_id(2);
  //   d.set_scca_id(2);

  //   AXT_DEBUG << "num drivers update: " << driver.update(d);;
  // }

  // find test
  {
    AXT_DEBUG << "###################### FIND";
    autoxtime::proto::Driver d;
    d.set_driver_id(9);
    d.set_first_name("junk");
    d.set_last_name("44dsfds");
    d.set_email("nick@test.com");
    d.set_phone_number("555-123-4567");
    d.set_msr_id(2);
    d.set_scca_id(2);
    std::vector<std::shared_ptr<autoxtime::proto::Driver> > drivers = driver.find(d);
    google::protobuf::util::JsonPrintOptions opts;
    opts.add_whitespace = true;
    std::string out;
    for (const std::shared_ptr<autoxtime::proto::Driver>& d : drivers)
    {
      google::protobuf::util::MessageToJsonString(*(d.get()), &out, opts);
      AXT_DEBUG << "Driver from database: " << QString::fromStdString(out);
    }
  }

  // find by ID
  {
    AXT_DEBUG << "###################### FIND BY ID";
    std::vector<std::shared_ptr<autoxtime::proto::Driver> > drivers = driver.findById(2);
    google::protobuf::util::JsonPrintOptions opts;
    opts.add_whitespace = true;
    std::string out;
    for (const std::shared_ptr<autoxtime::proto::Driver>& d : drivers)
    {
      google::protobuf::util::MessageToJsonString(*(d.get()), &out, opts);
      AXT_DEBUG << "Driver from database: " << QString::fromStdString(out);
    }
  }

  // list test
  {
    AXT_DEBUG << "###################### LIST";
    std::vector<std::shared_ptr<autoxtime::proto::Driver> > drivers = driver.list();
    for (const std::shared_ptr<autoxtime::proto::Driver>& d : drivers)
    {
      std::string out;
      google::protobuf::util::MessageToJsonString(*(d.get()), &out);
      AXT_DEBUG << "Driver from database: " << QString::fromStdString(out);
    }
  }

  return app.exec();
}
