#include <autoxtime/log/Logger.h>

// Qt
#include <QDebug>
#include <QtCore/QDir>

// std
#include <iostream>

AUTOXTIME_NAMESPACE_BEG

// only print file:line in ERROR and FATAL messages, otherwise it's just noise
const QString Logger::DEFAULT_MESSAGE_PATTERN = "[%{time}] %{if-debug}DEBUG%{endif}%{if-info}INFO%{endif}%{if-warning}WARNING%{endif}%{if-critical}ERROR - %{file}:%{line}%{endif}%{if-fatal}FATAL - %{file}:%{line}%{endif} - %{message}";

Logger::Logger(QObject* pParent)
    : QObject(pParent),
      mLogFilePath("log/autoxtime.log"),
      mpLogFile()
{
  qSetMessagePattern(DEFAULT_MESSAGE_PATTERN);
  qInstallMessageHandler(&Logger::messageHandler);
  openFile();
}

Logger& Logger::init(QObject* pParent)
{
  static Logger inst(pParent);
  return inst;
}

void Logger::openFile()
{
  // Create the path to our output file
  QFileInfo info(mLogFilePath);
  if (!QDir::root().mkpath(info.absolutePath()))
  {
    throw std::runtime_error("Unable to create path to log file \"" + mLogFilePath.toStdString()
                             + "\"");
  }

  mpLogFile = std::make_unique<QFile>(info.absoluteFilePath());
  if (!mpLogFile->open(QFile::WriteOnly))
  {
    throw std::runtime_error("Unable to open log file \"" +
                             info.absoluteFilePath().toStdString() +
                             "\" for writing");
  }
}

void Logger::write(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
  QString msg_formatted = qFormatLogMessage(type, context, msg);
  std::string msg_formatted_std = msg_formatted.toStdString() + "\n";
  // write to console
  std::cerr << msg_formatted_std << std::flush;

  // write to file
  mpLogFile->write(msg_formatted_std.c_str());
  mpLogFile->flush();

  // throw execption if fatal
  switch (type)
  {
    case QtDebugMsg:
    case QtInfoMsg:
    case QtWarningMsg:
    case QtCriticalMsg:
      break;
    case QtFatalMsg:
      throw std::runtime_error(msg_formatted_std);
      break;
  }
}

void Logger::messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
  init().write(type, context, msg);
}

AUTOXTIME_NAMESPACE_END
