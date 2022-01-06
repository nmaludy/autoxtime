#include <autoxtime/codec/ICodec.h>

#include <autoxtime/transport/ITransport.h>

AUTOXTIME_NAMESPACE_BEG

ICodec::ICodec(QObject* pParent)
    : ICodec(nullptr, pParent)
{}

ICodec::ICodec(ITransport* pTransport, QObject* pParent)
    : QObject(pParent),
      mpTransport(pTransport)
{
  if (mpTransport)
  {
    connect(mpTransport, &ITransport::dataRead,
            this,        &ICodec::handleDataRead);
  }
}

bool ICodec::decodeData(const QByteArray& data,
                        std::vector<std::shared_ptr<google::protobuf::Message>>& rMsgs)
{
  return false;
}

bool ICodec::encodeMessages(const std::vector<std::shared_ptr<google::protobuf::Message>>& msgs,
                            QByteArray& rData)
{
  return false;
}

void ICodec::handleDataRead(const QByteArray& data)
{
  std::vector<std::shared_ptr<google::protobuf::Message>> msgs;
  if (decodeData(data, msgs))
  {
    emit messagesRead(msgs);
  }
}

AUTOXTIME_NAMESPACE_END
