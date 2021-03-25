#ifndef AUTOXTIME_LOG_LOG
#define AUTOXTIME_LOG_LOG

#include <QtGlobal>
#include <QDebug>

#define AXT_INFO qInfo().nospace().noquote()
#define AXT_WARN qWarning().nospace().noquote()
#define AXT_ERROR qCritical().nospace().noquote()
#define AXT_DEBUG qDebug().nospace().noquote()
#define AXT_FATAL QDebug(QtFatalMsg).nospace().noquote()

inline QDebug operator<<(QDebug debug, const std::string& str)
{
  debug << QString::fromStdString(str);
  return debug;
}

#endif // AUTOXTIME_LOG_LOG
