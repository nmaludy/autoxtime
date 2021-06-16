#include <autoxtime/transport/FileIO.h>

// autoxtime
#include <autoxtime/config/ConfigStore.h>
#include <autoxtime/log/Log.h>

// Qt
#include <QFile>
#include <QTimer>

AUTOXTIME_NAMESPACE_BEG

FileIO::FileIO(QObject* pParent)
    : FileIO(QString(), "fileio", pParent)
{}

FileIO::FileIO(const QString& filePath,
               QObject* pParent)
    : FileIO(filePath, "fileio", pParent)
{}

FileIO::FileIO(const QString& filePath,
               const QString& configPath,
               QObject* pParent)
    : ITransport(pParent),
      mpFile(new QFile(this)),
      mFileMode(0)
{
  connect(this, &FileIO::readyRead,
          this, &FileIO::handleReadyRead);

  // read file name from config in case it wasn't passed in
  QString file = filePath;
  if (file.isEmpty())
  {
    file = ConfigStore::valueCast(configPath + "/path", QString());
  }
  mpFile->setFileName(file);

  QFile::OpenMode mode = QFile::NotOpen;
  // r, read
  // w, write
  // rw, read_write
  QString mode_str = ConfigStore::valueCast(configPath + "/mode", QString("rw")).toLower();
  if (mode_str == "rw" || mode_str == "read_write")
  {
    mode |= QFile::ReadWrite;
  }
  else if (mode_str == "r" || mode_str == "read")
  {
    mode |= QFile::ReadOnly;
  }
  else if (mode_str == "w" || mode_str == "write")
  {
    mode |= QFile::ReadOnly;
  }
  
  if (ConfigStore::valueCast(configPath + "/append", false))
  {
    mode |= QFile::Append;
  }
  if (ConfigStore::valueCast(configPath + "/truncate", false))
  {
    mode |= QFile::Truncate;
  }
  if (ConfigStore::valueCast(configPath + "/text", true))
  {
    mode |= QFile::Text;
  }
  if (ConfigStore::valueCast(configPath + "/unbuffered", false))
  {
    mode |= QFile::Unbuffered;
  }
  if (ConfigStore::valueCast(configPath + "/new_only", false))
  {
    mode |= QFile::NewOnly;
  }
  if (ConfigStore::valueCast(configPath + "/existing_only", false))
  {
    mode |= QFile::ExistingOnly;
  }
  mFileMode = static_cast<int>(mode);
}

bool FileIO::open()
{
  bool b_success = true;
  if (!mpFile->isOpen())
  {
    QFile::OpenMode mode = static_cast<QFile::OpenMode>(mFileMode);
    b_success = mpFile->open(mode);
    AXT_DEBUG << "FileTransport::tryOpen() - path=" << mpFile->fileName()
              << " mode=" << mode
              << " success=" << b_success; 
    if (b_success)
    {
      emit readyRead();
    }
    else
    {
      AXT_DEBUG << "FileTransport::tryOpen() - path=" << mpFile->fileName()
                << " error=" << mpFile->error()
                << " errorString=" << mpFile->errorString();
    }
  }
  return b_success;
}

void FileIO::handleReadyRead()
{
  AXT_DEBUG << "FileIO::handleReadyRead()";
  QByteArray data = read();
  emit dataRead(data);
}

QByteArray FileIO::read()
{
  AXT_DEBUG << "FileIO::read()";
  // TODO provide some different ways of reading the data
  // byte chunks  (with delay)
  // line-by-line (with delay)
  // delimiter (say '\r' or 'abc', with delay)
  return mpFile->readAll();
}

AUTOXTIME_NAMESPACE_END
