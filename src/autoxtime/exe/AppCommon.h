#ifndef AUTOXTIME_APP_APPCOMMON
#define AUTOXTIME_APP_APPCOMMON

#include <autoxtime/autoxtime.h>
#include <QString>

class QCommandLineParser;
class QCoreApplication;

AUTOXTIME_NAMESPACE_BEG

class AppCommon
{
 public:
  static void initCliParser(QCommandLineParser* pParser);
  static void init(QCoreApplication* pApp, const QString& name = QString());
};

AUTOXTIME_NAMESPACE_END

#endif // AUTOXTIME_APP_APPCOMMON
