#include "autoxtime/transport/SerialPortAsyncReader.h"

#include <iostream>
#include <QMetaEnum>
#include <QDebug>

SerialPortAsyncReader::SerialPortAsyncReader(const QString& portName,
                                             QObject* pParent)
    : ITransport(pParent),
      mPortName(portName),
      mSerialPort()
{
  // TODO configurable
  mSerialPort.setBaudRate(QSerialPort::Baud4800);
  mSerialPort.setDataBits(QSerialPort::Data8);
  mSerialPort.setParity(QSerialPort::NoParity);
  mSerialPort.setStopBits(QSerialPort::OneStop);
  // TODO should we cap the read buffer size? (default is unlimited)

  connect(&mSerialPort, &QSerialPort::readyRead, this, &SerialPortAsyncReader::handleReadyRead);
  connect(&mSerialPort, &QSerialPort::errorOccurred, this, &SerialPortAsyncReader::handleError);
  connect(&mTimer, &QTimer::timeout, this, &SerialPortAsyncReader::handleTimeout);

  tryConnect();
  mTimer.start(5000); // TODO configurable
}

QSerialPortInfo SerialPortAsyncReader::findPortInfo(const QString& portName)
{  
  QSerialPortInfo found_port;
  for (const QSerialPortInfo& serialPortInfo: QSerialPortInfo::availablePorts())
  {
    if (portName == serialPortInfo.portName())
    {
      found_port = serialPortInfo;
      qInfo().nospace() << "Found port: " << serialPortInfo.portName();
      break;
    }
  }

  if (found_port.isNull()) {
    qInfo().nospace() << "Couldn't find port with name: " << portName << "\n"
                      <<  "Available ports are: \n";
    for (const QSerialPortInfo& serialPortInfo: QSerialPortInfo::availablePorts())
    {
      const QString& description = serialPortInfo.description();
      const QString& manufacturer = serialPortInfo.manufacturer();
      const QString& serialNumber = serialPortInfo.serialNumber();
      qInfo().nospace() << "  Port name: " << serialPortInfo.portName()
                        << "\n    Location: " << serialPortInfo.systemLocation()
                        << "\n    Description: " << (!description.isEmpty() ? description : "N/A")
                        << "\n    Manufacturer: " << (!manufacturer.isEmpty() ? manufacturer : "N/A")
                        << "\n    Serial number: " << (!serialNumber.isEmpty() ? serialNumber : "N/A")
                        << "\n    Vendor Identifier: " << (serialPortInfo.hasVendorIdentifier()
                                                           ? QByteArray::number(serialPortInfo.vendorIdentifier(), 16)
                                                           : "N/A")
                        << "\n    Product Identifier: " << (serialPortInfo.hasProductIdentifier()
                                                            ? QByteArray::number(serialPortInfo.productIdentifier(), 16)
                                                            : "N/A");
    }
  }
  return found_port;
}

bool SerialPortAsyncReader::tryConnect()
{
  qInfo().nospace() << "Trying connect " << mPortName;
  resetError();

  if (!mSerialPort.isOpen())
  {
    qInfo().nospace() << "Trying to find port with name port " << mPortName;
    QSerialPortInfo found_port_info = findPortInfo(mPortName);
    if (found_port_info.isNull())
    {
      return false;
    }

    mSerialPort.setPort(found_port_info);
    qInfo().nospace() << "Trying to open port " << mPortName;
    if (!mSerialPort.open(QIODevice::ReadOnly))
    {
      qInfo().nospace() << "Failed to open port " << mPortName
                        << " , error: %2" << mSerialPort.errorString();
      mSerialPort.clearError();
      return false;
    }
  }
  else
  {  
    qInfo().nospace() << "Port is open: " << mPortName;
  }
  return true;
}

void SerialPortAsyncReader::resetError()
{
  if (mSerialPort.error() != QSerialPort::NoError)
  {
    qWarning().nospace() << "Error present at time of connection on the port " << mPortName
                         << " , error: %2" << mSerialPort.errorString()
                         << "\n"
                         << "Clearing error and trying again";
    
    if (mSerialPort.isOpen()) {
      mSerialPort.close();
    }
    mSerialPort.clearError();
  }
  else
  {
    qInfo().nospace() << "No error on port " << mPortName;
  }
}

void SerialPortAsyncReader::handleReadyRead()
{
  QByteArray read_data = mSerialPort.readAll();
  if (!read_data.isEmpty())
  {
    emit dataRead(read_data);
  }
  else
  {
    qInfo().nospace() << "handleReadyRead(): line isn't ready yet";
  }
}

void SerialPortAsyncReader::handleTimeout()
{
  // try to reconnect on a timeout
  tryConnect();
}

void SerialPortAsyncReader::handleError(QSerialPort::SerialPortError serialPortError)
{
  if (serialPortError != QSerialPort::NoError) {
    QString error_name =
        QMetaEnum::fromType<QSerialPort::SerialPortError>().valueToKey(serialPortError);
    qWarning().nospace() << "An " << error_name << " error occurred on port "
                         << mSerialPort.portName()
                         << ", error: "
                         << mSerialPort.errorString();
    resetError();
  }
}
