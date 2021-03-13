#include <autoxtime/autoxtime.h>
#include <autoxtime/config/ConfigStore.h>
// #include <autoxtime/server/src/autoxtimeserver.h>

// Cutelyst
// #include <Cutelyst/WSGI/wsgi.h>

// Qt
#include <QCoreApplication>
#include <QCommandLineParser>


void initCliParser(QCommandLineParser& parser) {
  parser.setApplicationDescription("Runs the autoxtime HTTP server.");
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addOption(QCommandLineOption(QStringList() << "c" << "config",
                                      "Path to the config file",
                                      "config",
                                      "./config/server.conf"));
}

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName(AUTOXTIME_ORG_NAME);
  QCoreApplication::setOrganizationDomain(AUTOXTIME_ORG_DOMAIN);
  QCoreApplication::setApplicationName("autoxtime_server");
  QCoreApplication::setApplicationVersion(AUTOXTIME_VERSION_STR);

  // Process the actual command line arguments given by the user
  QCommandLineParser parser;
  initCliParser(parser);
  parser.process(app);

  // initialize our config with our parser and our map before initializing any other
  // classes, this way other classes can just call ConfigStore::instance().value()
  // and get what they are after
  autoxtime::ConfigStore::init(parser.value("config"), &parser);

  // // create and run our Cutelyst server
  // CWSGI::WSGI server;

  // // Open HTTP/1.1 3000 port
  // // TODO configurable port
  // server.setHttpSocket({
  //     { QStringLiteral(":3000") },
  //   });
  // // TODO configurable buffer
  // server.setBufferSize(16393);
  // server.setMaster(true);
  // server.setAutoReload(true);
  // server.setReusePort(true);
  // server.setSocketTimeout(1);

  // server.exec(new AutoXTimeServer);
}
