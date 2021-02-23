#include <QCoreApplication>
#include <QCommandLineParser>
#include <QHash>
#include <QSerialPortInfo>
#include "autoxtime/codec/FarmtekCodec.h"
#include "autoxtime/config/ConfigStore.h"
#include "autoxtime/transport/SerialPortAsyncReader.h"


void initCliParser(QCommandLineParser& parser) {
  parser.setApplicationDescription("Reads data from timers (serial) and sends it to the server");
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addOption(QCommandLineOption(QStringList() << "c" << "config",
                                      "Path to the config file",
                                      "config",
                                      "./config/timers.conf"));

  parser.addOption(QCommandLineOption(QStringList() << "p" << "port",
                                      "Name of the serial port to read timer data from",
                                      "port"));
}

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("autoxtime");
  QCoreApplication::setOrganizationDomain("autoxtime.io");
  QCoreApplication::setApplicationName("autoxtime_timers");
  QCoreApplication::setApplicationVersion("1.0");
  
  // Process the actual command line arguments given by the user
  QCommandLineParser parser;
  initCliParser(parser);
  parser.process(app);

  // map from config option to CLI parser option name
  // this way we can "override" any config options with CLI options if we so choose
  QHash<QString, QStringList> config_cli_map;
  config_cli_map["serial/port"] = QStringList() << "p" << "port";
  config_cli_map["serial/baud_rate"] = QStringList() << "baud-rate";
  config_cli_map["serial/data_bits"] = QStringList() <<"data-bits";
  config_cli_map["serial/parity"] = QStringList() <<"parity";
  config_cli_map["serial/stop_bits"] = QStringList() <<"stop-bits";
  config_cli_map["serial/reconnect_interval_ms"] = QStringList() <<"reconnect-interval-ms";

  // initialize our config with our parser and our map before initializing any other
  // classes, this way other classes can just call ConfigStore::instance().value()
  // and get what they are after
  ConfigStore::init(parser.value("config"), &parser, config_cli_map);

  SerialPortAsyncReader serialPortReader(QString(), &app);
  FarmtekCodec farmtekCodec(&serialPortReader, &app);
  return app.exec();  
}
