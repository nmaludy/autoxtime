#include <autoxtime/autoxtime.h>
#include <autoxtime/exe/AppCommon.h>
// #include <autoxtime/codec/FarmtekCodec.h>
#include <autoxtime/codec/RaceAmericaTlinkCodec.h>
#include <autoxtime/config/ConfigStore.h>
#include <autoxtime/timing/TimingStateMachine.h>
#include <autoxtime/transport/FileIO.h>
#include <autoxtime/transport/SerialPortAsyncReader.h>

// Qt
#include <QCoreApplication>
#include <QHash>
#include <QSerialPortInfo>

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);
  autoxtime::AppCommon::init(&app, "autoxtime_timers");

  // transport
  autoxtime::ITransport* p_transport = nullptr;
  // p_transport = new autoxtime::SerialPortAsyncReader(QString(), &app);
  p_transport = new autoxtime::FileIO(&app);

  // codec
  autoxtime::ICodec* p_codec = nullptr;
  // autoxtime::FarmtekCodec farmtekCodec(&serialPortReader, &app);
  p_codec = new autoxtime::RaceAmericaTlinkCodec(p_transport, &app);

  // timing
  autoxtime::TimingStateMachine sm(p_codec, &app);

  // start data flow from transport
  p_transport->open();

  // start event loop
  return app.exec();
}
