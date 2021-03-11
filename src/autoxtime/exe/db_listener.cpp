#include <autoxtime/autoxtime.h>
#include <autoxtime/config/ConfigStore.h>
#include <autoxtime/db/DbListener.h>

// Qt
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QHash>
#include <QSerialPortInfo>

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

  autoxtime::DbListener db("autoxtime");
  return app.exec();  
}