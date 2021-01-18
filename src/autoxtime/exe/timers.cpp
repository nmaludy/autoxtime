#include <QCoreApplication>
#include <QSerialPortInfo>
#include "autoxtime/codec/FarmtekCodec.h"
#include "autoxtime/transport/SerialPortAsyncReader.h"

// TODO configurable
QString PORT_NAME("ttyUSB0");

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);
  SerialPortAsyncReader serialPortReader(PORT_NAME, &app);
  FarmtekCodec farmtekCodec(&serialPortReader, &app);
  return app.exec();  
}
