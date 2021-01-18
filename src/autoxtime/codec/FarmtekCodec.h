#ifndef AUTOXTIME_CODEC_FARMTEKCODEC
#define AUTOXTIME_CODEC_FARMTEKCODEC

#include <QObject>

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

#endif // AUTOXTIME_CODEC_FARMTEKCODEC
