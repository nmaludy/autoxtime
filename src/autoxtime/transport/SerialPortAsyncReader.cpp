#include "autoxtime/transport/SerialPortAsyncReader.h"

#include <iostream>
#include <QMetaEnum>

SerialPortAsyncReader::SerialPortAsyncReader(const QString& portName,
                                             QObject* pParent)
    : ITransport(pParent),
      mPortName(portName),
      mSerialPort(),
      mStandardOutput(stderr)
{
  mStandardOutput << "In constructor "
                  << mPortName
                  << Qt::endl;
  // TODO configurable
  mSerialPort.setBaudRate(QSerialPort::Baud4800);
  mSerialPort.setDataBits(QSerialPort::Data8);
  mSerialPort.setParity(QSerialPort::NoParity);
  mSerialPort.setStopBits(QSerialPort::OneStop);
  // TODO should we cap the read buffer size? (default is unlimited)

  connect(&mSerialPort, &QSerialPort::readyRead, this, &SerialPortAsyncReader::handleReadyRead);
  connect(&mSerialPort, &QSerialPort::errorOccurred, this, &SerialPortAsyncReader::handleError);
  connect(&mTimer, &QTimer::timeout, this, &SerialPortAsyncReader::handleTimeout);

  mStandardOutput << "Calling try connect "
                  << mPortName
                  << Qt::endl;
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
      mStandardOutput << "Found port: " << serialPortInfo.portName()
                      << Qt::endl;
      break;
    }
  }

  if (found_port.isNull()) {
    mStandardOutput << "Couldn't find port with name: " << portName << "\n"
                    <<  "Available ports are: \n";
    for (const QSerialPortInfo& serialPortInfo: QSerialPortInfo::availablePorts())
    {
      const QString& description = serialPortInfo.description();
      const QString& manufacturer = serialPortInfo.manufacturer();
      const QString& serialNumber = serialPortInfo.serialNumber();
      mStandardOutput << "  Port name: " << serialPortInfo.portName()
                      << "\n    Location: " << serialPortInfo.systemLocation()
                      << "\n    Description: " << (!description.isEmpty() ? description : "N/A")
                      << "\n    Manufacturer: " << (!manufacturer.isEmpty() ? manufacturer : "N/A")
                      << "\n    Serial number: " << (!serialNumber.isEmpty() ? serialNumber : "N/A")
                      << "\n    Vendor Identifier: " << (serialPortInfo.hasVendorIdentifier()
                                                         ? QByteArray::number(serialPortInfo.vendorIdentifier(), 16)
                                                         : "N/A")
                      << "\n    Product Identifier: " << (serialPortInfo.hasProductIdentifier()
                                                          ? QByteArray::number(serialPortInfo.productIdentifier(), 16)
                                                          : "N/A")
                      << Qt::endl;
    }
  }
  return found_port;
}

bool SerialPortAsyncReader::tryConnect()
{
  mStandardOutput << "Trying connect "
                  << mPortName
                  << "\n";
  resetError();

  if (!mSerialPort.isOpen())
  {
    mStandardOutput << "Trying to find port with name port "
                    << mPortName
                    << Qt::endl;
    QSerialPortInfo found_port_info = findPortInfo(mPortName);
    if (found_port_info.isNull())
    {
      return false;
    }

    mSerialPort.setPort(found_port_info);
    mStandardOutput << "Trying to open port "
                    << mPortName
                    << Qt::endl;
    if (!mSerialPort.open(QIODevice::ReadOnly))
    {
      mStandardOutput << "Failed to open port "
                      << mPortName
                      << " , error: %2"
                      << mSerialPort.errorString()
                      << Qt::endl;
      mSerialPort.clearError();
      return false;
    }
  }
  else
  {  
    mStandardOutput << "Port is open: "
                    << mPortName
                    << Qt::endl;
  }
  return true;
}

void SerialPortAsyncReader::resetError()
{
  if (mSerialPort.error() != QSerialPort::NoError)
  {
    mStandardOutput << "Error present at time of connection on the port "
                    << mPortName
                    << " , error: %2"
                    << mSerialPort.errorString()
                    << "\n"
                    << "Clearing error and trying again"
                    << Qt::endl;
    
    if (mSerialPort.isOpen()) {
      mSerialPort.close();
    }
    mSerialPort.clearError();
  }
  else
  {
    mStandardOutput << "No error on port "
                    << mPortName
                    << Qt::endl;
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
    mStandardOutput << "handleReadyRead(): line isn't ready yet" << Qt::endl;
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
    mStandardOutput << "An " << error_name << " error occurred on port "
                    << mSerialPort.portName()
                    << ", error: "
                    << mSerialPort.errorString()
                    << Qt::endl;
    resetError();
  }
}
