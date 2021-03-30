#ifndef AUTOXTIME_LOG_LOGGER
#define AUTOXTIME_LOG_LOGGER

#include <autoxtime/autoxtime.h>

// std
#include <memory>

// Qt
#include <QObject>
#include <QtCore/QString>
#include <QtCore/QFile>

AUTOXTIME_NAMESPACE_BEG

class Logger : public QObject
{
  Q_OBJECT

 public:
  static const QString DEFAULT_PATH;
  static const QString DEFAULT_MESSAGE_PATTERN;
  static const QString PATH_TOKEN_DATE;

  static Logger& init(QObject* pParent = nullptr);
  static void messageHandler(QtMsgType type,
                             const QMessageLogContext& context,
                             const QString& msg);
  void write(QtMsgType type,
             const QMessageLogContext& context,
             const QString& msg);
  void config();

 private:
  explicit Logger(QObject* pParent = nullptr);
  void openFile();

  QString mLogFilePath;
  std::unique_ptr<QFile> mpLogFile;
};

AUTOXTIME_NAMESPACE_END

#endif // AUTOXTIME_LOG_LOGGER
