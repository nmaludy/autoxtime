#include <autoxtime/codec/FarmtekCodec.h>
#include <autoxtime/transport/ITransport.h>
#include <autoxtime/log/Log.h>

AUTOXTIME_NAMESPACE_BEG

// 13 byte fixed format message
// First character is either B or R (although some modes of the Polaris unit may also give you a S and F, Start finish)
// 2 signals are sent when an object passes completely through a light beam.
// The beam break timestamp of the is sent with "B"
// then the beam restore timestamp is sent with "R"
// The 2nd character is a number 1-4 representing the eye number
// Character 3-12 is a integer of the current "time" of the Polaris box when the signal is received from the eye
// Character 13 is a carriage return. which is ASCII decimal (13) or hex "D"
//
// Setting this to 12 because the last byte is a CR that we are splitting on
const int FARMTEK_MSG_EXPECTED_SIZE = 12;

FarmtekCodec::FarmtekCodec(ITransport* pTransport, QObject* pParent)
    : QObject(pParent),
      mpTransport(pTransport),
      mDataBuffer()
{
  connect(mpTransport, &ITransport::dataRead, this, &FarmtekCodec::handleDataRead);
}

void FarmtekCodec::handleDataRead(const QByteArray& data)
{
  // for (int i = 0; i < read_data.size(); ++i) {
  //    AXT_INFO << "handleReadyRead(): read data[" << i << "]: " << (int)read_data[i];
  // }
  mDataBuffer.append(data);

  // yes, only search the newest line added so we don't have to scan the entire buffer
  // for stuff we already know doesn't have a newline

  // TODO make this more efficient so we're not scanning multiple times
  // TODO should we split up the data here or should we just pass it on to the codec "raw"
  //     then let the codec buffer and figure it out?
  if (data.contains('\r'))
  {
    QList<QByteArray> lines = mDataBuffer.split('\r');
    mDataBuffer = lines.takeLast();
    for (const QByteArray& line : lines) {
      if (line.size() != FARMTEK_MSG_EXPECTED_SIZE)
      {
        AXT_WARNING
            << "Received Farmtek data that is not the correct size. "
            << "Expected=" << FARMTEK_MSG_EXPECTED_SIZE
            << " Received=" << line.size()
            << " Data=" << line;
        continue;
      }
      AXT_INFO << "Received a Farmtek string of correct size: " << line;

      // byte  0 = msg type, either "B" for break or "R" for reset
      char msg_type = line[0];
      // byte 1 = string containing "1", "2", "3", "4" to indicate eye number
      int eye = QString(line[1]).toInt();
      // bytes 2-11 = timestamp in 0.125us resolution
      QString timestamp_str = line.mid(2, 10);
      double timestamp_125us = timestamp_str.toDouble();
      double timestamp_s = timestamp_125us / 8000.0;

      AXT_INFO
          << "msg_type=" << msg_type
          << " eye=" << eye
          << " timestamp_str=" << timestamp_str
          << " timestamp_s=" << timestamp_s;
    }
  }

}

AUTOXTIME_NAMESPACE_END
