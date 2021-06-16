#ifndef AUTOXTIME_TRANSPORT_SERIALPORTASYNCREADER
#define AUTOXTIME_TRANSPORT_SERIALPORTASYNCREADER

#include <autoxtime/autoxtime.h>
#include <autoxtime/transport/ITransport.h>

#include <QString>
#include <QSerialPortInfo>

class QSerialPort;
class QTimer;
class QFile;

AUTOXTIME_NAMESPACE_BEG

class SerialPortAsyncReader : public ITransport
{
  Q_OBJECT
 public:
  SerialPortAsyncReader(const QString& portName = QString(), QObject* pParent = nullptr);
  virtual ~SerialPortAsyncReader() = default;

  virtual bool open() override;
             
 private slots:
  void handleReadyRead();
  void handleTimeout();
  void handleError();

 private:
  QSerialPortInfo findPortInfo(const QString& portName);
  void resetError();

  QString mPortName;
  QSerialPort* mpSerialPort;
  QTimer* mpTimer;
  QFile* mpDataLogFile;
  bool mDataLogOpened;
};

AUTOXTIME_NAMESPACE_END

#endif // AUTOXTIME_TRANSPORT_SERIALPORTASYNCREADER
