#ifndef AUTOXTIME_CONFIG_CONFIGSTORE
#define AUTOXTIME_CONFIG_CONFIGSTORE

#include <autoxtime.h>

// Qt
#include <QDebug>
#include <QHash>
#include <QObject>
#include <QSettings>
#include <QStringList>

class QCommandLineParser;

AUTOXTIME_NAMESPACE_BEG

class ConfigStore : public QObject
{
  Q_OBJECT

 public:
  typedef QHash<QString, QStringList> ConfigCliOptsMap;
  
  static ConfigStore& init(const QString& filePath = QString(),
                           QCommandLineParser* pParser = nullptr,
                           const ConfigCliOptsMap& configCliMap = ConfigCliOptsMap());

  static const QString& filePath();

  static QVariant value(const QString& key, const QVariant& defaultValue = QVariant());

  template <typename T>
  static T valueCast(const QString& key, const T& defaultValue)
  {
    QVariant data = value(key, defaultValue);
    T value_t = data.value<T>();
    qInfo().nospace() << "  cast<T> value = " << value_t;
    return value_t;
  }
  static void setValue(const QString& key, const QVariant& value);
      
 private:
  ConfigStore(const QString& filePath,
              QCommandLineParser* pParser,
              const QHash<QString, QStringList>& configCliMap,
              QObject* pParent = nullptr);

  QString mFilePath;
  QSettings mSettings;
  QCommandLineParser* mpParser;
  ConfigCliOptsMap mConfigCliMap;
};

AUTOXTIME_NAMESPACE_END

#endif // AUTOXTIME_CONFIG_CONFIGSTORE
