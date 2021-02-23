#include "autoxtime/config/ConfigStore.h"
#include <QCommandLineParser>
#include <QDebug>

ConfigStore::ConfigStore(const QString& filePath,
                         QCommandLineParser* pParser,
                         const ConfigCliOptsMap& configCliMap,
                         QObject* pParent)
    : QObject(pParent),
      mFilePath(filePath),
      mSettings(mFilePath, QSettings::IniFormat, this),
      mpParser(pParser),
      mConfigCliMap(configCliMap)
{}

ConfigStore& ConfigStore::init(const QString& filePath,
                               QCommandLineParser* pParser,
                               const ConfigCliOptsMap& configCliMap)
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
  auto configCliMap = ConfigStore::init().mConfigCliMap;
  auto pParser = ConfigStore::init().mpParser;

  ConfigCliOptsMap::const_iterator iter = configCliMap.find(key);
  QVariant value;
  if (iter != configCliMap.end()) {
    const QStringList& cli_opt_names = *iter;

    // compare the CLI option map names to the options passed in on the command line
    QStringList option_names = pParser->optionNames();
    QStringList::const_iterator c_iter =
        std::find_first_of(cli_opt_names.begin(), cli_opt_names.end(),
                           option_names.begin(), option_names.end());

    // we found a matching option, use that as an override
    if (c_iter != cli_opt_names.end()) {
      const QString& cli_opt = *c_iter;
      value = QVariant(pParser->value(cli_opt));
      QString cli_opt_pretty = (cli_opt.size() == 1) ? "-" + cli_opt : "--" + cli_opt;
      qInfo().nospace() << "config[" << key << "] cli override ("
                        << cli_opt_pretty
                        << ") = " << value;
      return value;
    }
  }

  value = ConfigStore::init().mSettings.value(key, defaultValue);
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
