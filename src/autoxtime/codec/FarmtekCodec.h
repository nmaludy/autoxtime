#ifndef AUTOXTIME_CODEC_FARMTEKCODEC
#define AUTOXTIME_CODEC_FARMTEKCODEC

#include <autoxtime/autoxtime.h>
#include <autoxtime/codec/ICodec.h>

// Qt
#include <QObject>

AUTOXTIME_NAMESPACE_BEG

class FarmtekCodec : public ICodec
{
  Q_OBJECT

 public:
  explicit FarmtekCodec(ITransport* pTransport, QObject* pParent = nullptr);
  virtual ~FarmtekCodec() = default;

  virtual bool decodeData(const QByteArray& data,
                          std::vector<std::shared_ptr<google::protobuf::Message>>& rMsgs) override;

 private:
  QByteArray mDataBuffer;
};

AUTOXTIME_NAMESPACE_END

#endif // AUTOXTIME_CODEC_FARMTEKCODEC
