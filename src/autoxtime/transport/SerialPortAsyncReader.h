#ifndef AUTOXTIME_TRANSPORT_SERIALPORTASYNCREADER
#define AUTOXTIME_TRANSPORT_SERIALPORTASYNCREADER

#include <QSerialPortInfo>
#include <QSerialPort>
#include <QTextStream>
#include <QTimer>

#include "autoxtime/transport/ITransport.h"

class SerialPortAsyncReader : public ITransport
{
  Q_OBJECT
 public:
  SerialPortAsyncReader(const QString& portName, QObject* pParent = nullptr);
                                                                            
 private slots:
  void handleReadyRead();
  void handleTimeout();
  void handleError(QSerialPort::SerialPortError error);
  
 private:
  bool tryConnect();
  QSerialPortInfo findPortInfo(const QString& portName);
  void resetError();
  
  QString mPortName;
  QSerialPort mSerialPort;
  QTextStream mStandardOutput;
  QTimer mTimer;
};

#endif // AUTOXTIME_TRANSPORT_SERIALPORTASYNCREADER