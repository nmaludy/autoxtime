#include <autoxtime/exe/AppCommon.h>

// autoxtime
#include <autoxtime/log/Logger.h>
#include <autoxtime/config/ConfigStore.h>

// Qt
#include <QCommandLineParser>
#include <QCoreApplication>

AUTOXTIME_NAMESPACE_BEG

void AppCommon::initCliParser(QCommandLineParser* pParser)
{
  pParser->setApplicationDescription("autoxtime application");
  pParser->addHelpOption();
  pParser->addVersionOption();
  pParser->addOption(QCommandLineOption(QStringList() << "c" << "config",
                                        "Path to the config file",
                                        "config",
                                        "./config/autoxtime.conf"));
}

void AppCommon::init(QCoreApplication* pApp, const QString& name)
{
  QCoreApplication::setOrganizationName(AUTOXTIME_ORG_NAME);
  QCoreApplication::setOrganizationDomain(AUTOXTIME_ORG_DOMAIN);
  if (!name.isEmpty())
  {
    QCoreApplication::setApplicationName(name);
  }
  QCoreApplication::setApplicationVersion(AUTOXTIME_VERSION_STR);

  // Process the actual command line arguments given by the user
  QCommandLineParser parser;
  AppCommon::initCliParser(&parser);
  parser.process(*pApp);

  // initialize our logger so we can log things (must come first because config logs thigns)
  autoxtime::Logger::init();

  // must come after logger is setup
  // initialize our config with our parser and our map before initializing any other
  // classes, this way other classes can just call ConfigStore::instance().value()
  // and get what they are after
  //
  // example showing config -> CLI mappings for overrides
  // // map from config option to CLI parser option name
  // // this way we can "override" any config options with CLI options if we so choose
  // QHash<QString, QStringList> config_cli_map;
  // config_cli_map["serial/port"] = QStringList() << "p" << "port";
  // config_cli_map["serial/baud_rate"] = QStringList() << "baud-rate";
  // config_cli_map["serial/data_bits"] = QStringList() <<"data-bits";
  // config_cli_map["serial/parity"] = QStringList() <<"parity";
  // config_cli_map["serial/stop_bits"] = QStringList() <<"stop-bits";
  // config_cli_map["serial/reconnect_interval_ms"] = QStringList() <<"reconnect-interval-ms";
  autoxtime::ConfigStore::init(parser.value("config"), &parser);

  // configure our logger based on stuff from the config
  autoxtime::Logger::init().config();
}

AUTOXTIME_NAMESPACE_END
