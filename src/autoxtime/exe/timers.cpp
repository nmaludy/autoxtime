#include <QCoreApplication>
#include <QSerialPortInfo>
#include "autoxtime/codec/FarmtekCodec.h"
#include "autoxtime/config/ConfigStore.h"
#include "autoxtime/transport/SerialPortAsyncReader.h"

// TODO configurable
QString PORT_NAME("ttyUSB0");

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("autoxtime");
  QCoreApplication::setOrganizationDomain("autoxtime.io");
  QCoreApplication::setApplicationName("autoxtime_timers");
  QCoreApplication::setApplicationVersion("1.0");
  ConfigStore::instance("./settings.conf");
  ConfigStore::instance().setValue("serial_port_name", "ttyUSB0");
  
  SerialPortAsyncReader serialPortReader(PORT_NAME, &app);
  FarmtekCodec farmtekCodec(&serialPortReader, &app);
  return app.exec();  
}
