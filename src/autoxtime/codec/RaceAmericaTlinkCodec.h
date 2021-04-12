#ifndef AUTOXTIME_CODEC_RACEAMERICATLINKCODEC
#define AUTOXTIME_CODEC_RACEAMERICATLINKCODEC

#include <autoxtime/autoxtime.h>

// Qt
#include <QObject>

AUTOXTIME_NAMESPACE_BEG

class ITransport;

class RaceAmericaTlinkCodec : public QObject
{
  Q_OBJECT

 public:
  explicit RaceAmericaTlinkCodec(ITransport* pTransport, QObject* pParent = nullptr);

 public slots:
  void handleDataRead(const QByteArray& data);

 private:
  ITransport* mpTransport;
  QByteArray mDataBuffer;
};

AUTOXTIME_NAMESPACE_END

#endif // AUTOXTIME_CODEC_RACEAMERICATLINKCODEC
