#ifndef AUTOXTIME_CODEC_RACEAMERICATLINKCODEC
#define AUTOXTIME_CODEC_RACEAMERICATLINKCODEC

#include <autoxtime/autoxtime.h>
#include <autoxtime/codec/ICodec.h>

// Qt
#include <QObject>

AUTOXTIME_NAMESPACE_BEG

class RaceAmericaTlinkCodec : public ICodec
{
  Q_OBJECT

 public:
  explicit RaceAmericaTlinkCodec(ITransport* pTransport, QObject* pParent = nullptr);
  virtual ~RaceAmericaTlinkCodec() = default;

  virtual bool decodeData(const QByteArray& data,
                          std::vector<std::shared_ptr<google::protobuf::Message>>& rMsgs) override;

 private:
  // Setting this to 10 because the last byte is a CR that we are splitting on
  static constexpr int MSG_EXPECTED_SIZE = 10;
  
  QByteArray mDataBuffer;
};

AUTOXTIME_NAMESPACE_END

#endif // AUTOXTIME_CODEC_RACEAMERICATLINKCODEC
