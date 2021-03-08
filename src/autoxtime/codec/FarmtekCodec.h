#ifndef AUTOXTIME_CODEC_FARMTEKCODEC
#define AUTOXTIME_CODEC_FARMTEKCODEC

#include <autoxtime/autoxtime.h>

// Qt
#include <QObject>

AUTOXTIME_NAMESPACE_BEG

class ITransport;

class FarmtekCodec : public QObject
{
  Q_OBJECT

 public:
  FarmtekCodec(ITransport* pTransport, QObject* pParent = nullptr);

 public slots:
  void handleDataRead(const QByteArray& data);

 private:
  ITransport* mpTransport;
  QByteArray mDataBuffer;
};

AUTOXTIME_NAMESPACE_END

#endif // AUTOXTIME_CODEC_FARMTEKCODEC
