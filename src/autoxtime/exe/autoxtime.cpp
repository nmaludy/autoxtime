#include <autoxtime.h>
#include <autoxtime/ui/MainWindow.h>
#include <autoxtime/config/ConfigStore.h>
#include <autoxtime/transport/MotorSportsRegReader.h>

// Qt
#include <QApplication>
#include <QCommandLineParser>


void initCliParser(QCommandLineParser& parser) {
  parser.setApplicationDescription("Reads data from timers (serial) and sends it to the server");
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addOption(QCommandLineOption(QStringList() << "c" << "config",
                                      "Path to the config file",
                                      "config",
                                      "./config/autoxtime.conf"));
}

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QApplication::setOrganizationName(AUTOXTIME_ORG_NAME);
  QApplication::setOrganizationDomain(AUTOXTIME_ORG_DOMAIN);
  QApplication::setApplicationName("autoxtime");
  QApplication::setApplicationVersion(AUTOXTIME_VERSION_STR);
  
  // Process the actual command line arguments given by the user
  QCommandLineParser parser;
  initCliParser(parser);
  parser.process(app);

  // initialize our config with our parser and our map before initializing any other
  // classes, this way other classes can just call ConfigStore::instance().value()
  // and get what they are after
  autoxtime::ConfigStore::init(parser.value("config"), &parser);

  // autoxtime::MainWindow w;
  // w.show();

  autoxtime::MotorSportsRegReader reader;
  reader.readFile("");
  return app.exec();  
}
