#include "autoxtime/config/ConfigStore.h"
#include <QCommandLineParser>
#include <QDebug>

ConfigStore::ConfigStore(const QString& filePath,
                         QCommandLineParser* pParser,
                         const QHash<QString, QString>& configCliMap,
                         QObject* pParent)
    : QObject(pParent),
      mFilePath(filePath),
      mSettings(mFilePath, QSettings::IniFormat, this),
      mpParser(pParser),
      mConfigCliMap(configCliMap)
{}

ConfigStore& ConfigStore::init(const QString& filePath,
                               QCommandLineParser* pParser,
                               const QHash<QString, QString>& configCliMap)
{
  static ConfigStore inst(filePath, pParser, configCliMap);
  return inst;
}

const QString& ConfigStore::filePath()
{
  return ConfigStore::init().mFilePath;
}

QVariant ConfigStore::value(const QString& key,
                            const QVariant& defaultValue)
{
  QHash<QString, QString>::const_iterator iter = ConfigStore::init().mConfigCliMap.find(key);
  QVariant value;
  if (iter != ConfigStore::init().mConfigCliMap.end() &&
      ConfigStore::init().mpParser->optionNames().contains(*iter) &&
      ConfigStore::init().mpParser->isSet(*iter))
  {
    value = QVariant(ConfigStore::init().mpParser->value(*iter));
  }
  else
  {
    value = ConfigStore::init().mSettings.value(key, defaultValue);
  }
  qInfo().nospace() << "config[" << key << "] read = " << value;
  return value;
}

void ConfigStore::setValue(const QString& key,
                           const QVariant& value)
{
  qInfo().nospace() << "config[" << key << "] write = " << value;
  ConfigStore::init().mSettings.setValue(key, value);
  ConfigStore::init().mSettings.sync(); // save changes to disk
}
