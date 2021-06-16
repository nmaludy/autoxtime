#include <autoxtime/codec/FarmtekCodec.h>
#include <autoxtime/transport/ITransport.h>
#include <autoxtime/log/Log.h>

#include <qdebug.h>                  // for QDebug
#include <qlist.h>                   // for QList

AUTOXTIME_NAMESPACE_BEG

FarmtekCodec::FarmtekCodec(ITransport* pTransport, QObject* pParent)
    : ICodec(pTransport, pParent),
      mDataBuffer()
{}

bool FarmtekCodec
::decodeData(const QByteArray& data,
             std::vector<std::shared_ptr<google::protobuf::Message>>& rMsgs)
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
  bool b_msgs = false;
  if (data.contains('\r'))
  {
    QList<QByteArray> lines = mDataBuffer.split('\r');
    mDataBuffer = lines.takeLast();
    for (const QByteArray& line : lines)
    {
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
      b_msgs = true;
    }
  }
  return b_msgs;
}

AUTOXTIME_NAMESPACE_END
