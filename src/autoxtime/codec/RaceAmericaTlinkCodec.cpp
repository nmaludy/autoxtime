#include <autoxtime/codec/RaceAmericaTlinkCodec.h>
#include <autoxtime/transport/ITransport.h>
#include <autoxtime/log/Log.h>

AUTOXTIME_NAMESPACE_BEG

// 11 (10 data bytes + "\r") byte fixed format message
//
// First character is one of the following:
//   - "A" : start timer
//   - "B" : finish timer
//   - "C" : sector 1/2 timer
//   - "D" : sector 1/2 timer
//   - "Z" : Zome internal message about battery strength
//
// [A\B|C|D] format:
//   Bssssssfff\r
//   - s = seconds number
//   - f = sections fraction number
//
// examples:
// C003369.891
// D003390.830
// A003508.071
// A003590.904
// C003614.554
// D003641.335
// B003655.005
// A003626566
// C003646609

// Z format:
//   Z?????????\r


RaceAmericaTlinkCodec::RaceAmericaTlinkCodec(ITransport* pTransport, QObject* pParent)
    : ICodec(pTransport, pParent),
      mDataBuffer()
{}


bool RaceAmericaTlinkCodec
::decodeData(const QByteArray& data,
             std::vector<std::shared_ptr<google::protobuf::Message>>& rMsgs)
{
  AXT_DEBUG << "RaceAmericaTlinkCodec::handleDataRead " << data.size();
  mDataBuffer.append(data);

  // yes, only search the newest line added so we don't have to scan the entire buffer
  // for stuff we already know doesn't have a newline

  // TODO make this more efficient so we're not scanning multiple times
  // TODO should we split up the data here or should we just pass it on to the codec "raw"
  //     then let the codec buffer and figure it out?
  // AXT_DEBUG << "RaceAmericaTlinkCodec::handleDataRead " << (int)data[0];
  bool b_msgs = false;
  if (data.contains('\r'))
  {
    AXT_DEBUG << "RaceAmericaTlinkCodec::handleDataRead - data contains cr";
    QList<QByteArray> lines = mDataBuffer.split('\r');
    mDataBuffer = lines.takeLast();
    for (const QByteArray& line : lines)
    {
      if (line.size() != MSG_EXPECTED_SIZE)
      {
        AXT_WARNING
            << "Received RaceAmericaTlink data that is not the correct size. "
            << "Expected=" << MSG_EXPECTED_SIZE
            << " Received=" << line.size()
            << " Data=" << line;
        continue;
      }
      // AXT_DEBUG << "Received a RaceAmericaTlink string of correct size: " << line;

      // byte  0 = eye / message type
      char msg_type = line[0];
      if (msg_type == 'Z')
      {
        // ignore internal "Z" messages
        continue;
      }
      // byte 1-6 = string containing whole part of the time (seconds)
      QString whole_str = line.mid(1, 6);
      double whole = whole_str.toInt();
      // bytes 7-10 = string containing fractional part of the time (seconds)
      QString fraction_str = line.mid(7, 10);
      // prepend "0." so we get a fractional number when parsing double
      // TODO figure out a fancier way to convert this number into a fraction without
      // having to mess with the string parsing
      double fraction = QString("0." + fraction_str).toDouble();

      double timestamp = whole + fraction;
      AXT_INFO
          << "msg_type=" << msg_type
          << " whole_str=" << whole_str
          << " whole=" << whole
          << " fraction_str=" << fraction_str
          << " fraction=" << fraction
          << " timestamp=" << qSetRealNumberPrecision(10) << timestamp;
      b_msgs = true;
      // TODO
      // rMsgs.push_back();
    }
  }
  return b_msgs;
}

AUTOXTIME_NAMESPACE_END
