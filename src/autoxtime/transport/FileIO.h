#ifndef AUTOXTIME_TRANSPORT_FILEIO
#define AUTOXTIME_TRANSPORT_FILEIO

#include <autoxtime/autoxtime.h>
#include <autoxtime/transport/ITransport.h>

class QFile;
class QTimer;

AUTOXTIME_NAMESPACE_BEG

class FileIO : public ITransport
{
  Q_OBJECT
 public:
  explicit FileIO(QObject* pParent = nullptr);
  explicit FileIO(const QString& filePath,
                  QObject* pParent = nullptr);
  explicit FileIO(const QString& filePath,
                  const QString& configPath,
                  QObject* pParent = nullptr);

  virtual ~FileIO() = default;

  virtual bool open() override;
  QByteArray read();

 signals:
  void readyRead();

 protected slots:
  void handleReadyRead();

 private:

  QFile* mpFile;
  int mFileMode;
};

AUTOXTIME_NAMESPACE_END

#endif // AUTOXTIME_TRANSPORT_FILEIO
