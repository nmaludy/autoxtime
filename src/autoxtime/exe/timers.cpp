#include <autoxtime/autoxtime.h>
#include <autoxtime/exe/AppCommon.h>
// #include <autoxtime/codec/FarmtekCodec.h>
#include <autoxtime/codec/RaceAmericaTlinkCodec.h>
#include <autoxtime/config/ConfigStore.h>
#include <autoxtime/transport/SerialPortAsyncReader.h>

// Qt
#include <QCoreApplication>
#include <QHash>
#include <QSerialPortInfo>

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);
  autoxtime::AppCommon::init(&app, "autoxtime_timers");
  autoxtime::SerialPortAsyncReader serialPortReader(QString(), &app);
  // autoxtime::FarmtekCodec farmtekCodec(&serialPortReader, &app);
  autoxtime::RaceAmericaTlinkCodec codec(&serialPortReader, &app);
  return app.exec();
}
