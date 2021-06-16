#include <autoxtime/transport/SerialPortAsyncReader.h>

// autoxtime
#include <autoxtime/config/ConfigStore.h>
#include <autoxtime/log/Log.h>

// Qt
#include <QFile>
#include <QMetaEnum>
#include <QSerialPort>
#include <QTimer>

// other
#include <algorithm>
#include <iostream>

AUTOXTIME_NAMESPACE_BEG

SerialPortAsyncReader::SerialPortAsyncReader(const QString& portName,
                                             QObject* pParent)
    : ITransport(pParent),
      mPortName(portName),
      mpSerialPort(new QSerialPort(this)),
      mpTimer(new QTimer(this)),
      mpDataLogFile(new QFile(ConfigStore::valueCast("serial/data_log_file", QString()), this)),
      mDataLogOpened(false)
{
  if (mPortName.isNull())
  {
    mPortName = ConfigStore::value("serial/port").toString();
  }
  mpSerialPort->setBaudRate(ConfigStore::valueCast("serial/baud_rate", QSerialPort::Baud4800));
  mpSerialPort->setDataBits(ConfigStore::valueCast("serial/data_bits", QSerialPort::Data8));
  mpSerialPort->setParity(ConfigStore::valueCast("serial/parity", QSerialPort::NoParity));
  mpSerialPort->setStopBits(ConfigStore::valueCast("serial/stop_bits", QSerialPort::OneStop));
  // TODO should we cap the read buffer size? (default is unlimited)

  mDataLogOpened = mpDataLogFile->open(QFile::WriteOnly | QFile::Text | QFile::Append);

  connect(mpSerialPort, &QSerialPort::readyRead,
          this,         &SerialPortAsyncReader::handleReadyRead);
  connect(mpSerialPort, &QSerialPort::errorOccurred,
          this,         &SerialPortAsyncReader::handleError);
  connect(mpTimer, &QTimer::timeout,
          this,     &SerialPortAsyncReader::handleTimeout);
}

QSerialPortInfo SerialPortAsyncReader::findPortInfo(const QString& portName)
{
  QList<QSerialPortInfo> available_ports = QSerialPortInfo::availablePorts();
  QList<QSerialPortInfo>::iterator iter = std::find_if(
      available_ports.begin(),
      available_ports.end(),
      [=](const QSerialPortInfo& serial_port_info) {
        return serial_port_info.portName() == portName;
      });

  QSerialPortInfo found_port;
  if (iter != available_ports.end())
  {
    found_port = *iter;
    AXT_INFO << "Found port: " << found_port.portName();
  }
  else
  {
    AXT_INFO << "Couldn't find port with name: " << portName << "\n"
             <<  "Available ports are: \n";
    for (const QSerialPortInfo& serialPortInfo: available_ports)
    {
      const QString& description = serialPortInfo.description();
      const QString& manufacturer = serialPortInfo.manufacturer();
      const QString& serialNumber = serialPortInfo.serialNumber();
      AXT_INFO << "  Port name: " << serialPortInfo.portName()
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

bool SerialPortAsyncReader::open()
{
  AXT_INFO << "Trying connect " << mPortName;
  resetError();

  if (!mpSerialPort->isOpen())
  {
    AXT_INFO << "Trying to find port with name port " << mPortName;
    QSerialPortInfo found_port_info = findPortInfo(mPortName);
    if (found_port_info.isNull())
    {
      return false;
    }

    mpSerialPort->setPort(found_port_info);
    AXT_INFO << "Trying to open port " << mPortName;
    if (!mpSerialPort->open(QIODevice::ReadOnly))
    {
      AXT_ERROR << "Failed to open port " << mPortName
                << " , error: %2" << mpSerialPort->errorString();
      mpSerialPort->clearError();
      return false;
    }
  }
  else
  {
    AXT_INFO << "Port is open: " << mPortName;
  }

  // try to reconnect on a timeout
  if (!mpTimer->isActive())
  {
    mpTimer->start(ConfigStore::valueCast("serial/reconnect_interval_ms", 5000));
  }
  return true;
}

void SerialPortAsyncReader::resetError()
{
  if (mpSerialPort->error() != QSerialPort::NoError)
  {
    AXT_ERROR << "Error present at time of connection on the port " << mPortName
              << " , error: %2" << mpSerialPort->errorString()
              << "\n"
              << "Clearing error and trying again";

    if (mpSerialPort->isOpen())
    {
      mpSerialPort->close();
    }
    mpSerialPort->clearError();
  }
  else
  {
    AXT_INFO << "No error on port " << mPortName;
  }
}

void SerialPortAsyncReader::handleReadyRead()
{
  QByteArray read_data = mpSerialPort->readAll();
  if (!read_data.isEmpty())
  {
    if (mDataLogOpened)
    {
      mpDataLogFile->write(read_data);
      mpDataLogFile->flush();
    }
    emit dataRead(read_data);
  }
  else
  {
    AXT_INFO << "handleReadyRead(): line isn't ready yet";
  }
}

void SerialPortAsyncReader::handleTimeout()
{
  open();
}

void SerialPortAsyncReader::handleError()
{
  QSerialPort::SerialPortError serialPortError = mpSerialPort->error();
  if (serialPortError != QSerialPort::NoError)
  {
    QString error_name =
        QMetaEnum::fromType<QSerialPort::SerialPortError>().valueToKey(serialPortError);
    AXT_ERROR << "An " << error_name << " error occurred on port "
              << mpSerialPort->portName()
              << ", error: "
              << mpSerialPort->errorString();
    resetError();
  }
}

AUTOXTIME_NAMESPACE_END
