#ifndef AUTOXTIME_CODEC_ICODEC
#define AUTOXTIME_CODEC_ICODEC

#include <autoxtime/autoxtime.h>
#include <QObject>

#include <memory>
#include <vector>

namespace autoxtime { class ITransport; }
namespace google { namespace protobuf { class Message; } }

AUTOXTIME_NAMESPACE_BEG

class ICodec : public QObject
{
  Q_OBJECT

 public:
  explicit ICodec(QObject* pParent = nullptr);
  explicit ICodec(ITransport* pTransport, QObject* pParent = nullptr);
  virtual ~ICodec() = default;

  inline ITransport* transport();

  virtual bool decodeData(const QByteArray& data,
                          std::vector<std::shared_ptr<google::protobuf::Message>>& rMsgs);
  virtual bool encodeMessages(const std::vector<std::shared_ptr<google::protobuf::Message>>& msgs,
                              QByteArray& rData);

 public slots:
  virtual void handleDataRead(const QByteArray& data);

 signals:
  void messagesRead(const std::vector<std::shared_ptr<google::protobuf::Message>>& msgs);

 protected:
  ITransport* mpTransport;
};

AUTOXTIME_NAMESPACE_END

#endif // AUTOXTIME_CODEC_ICODEC
