#include "autoxtime/config/ConfigStore.h"

ConfigStore::ConfigStore(const QString& filePath,QObject* pParent)
    : QObject(pParent),
      mFilePath(filePath),
      mSettings(mFilePath, QSettings::IniFormat, this)
{}

ConfigStore& ConfigStore::instance(const QString& filePath)
{
  static ConfigStore inst(filePath);
  return inst;
}

const QString& ConfigStore::filePath() const
{
  return mPath;
}

QVariant ConfigStore::value(const QString& key, const QVariant& defaultValue) const
{
  return mSettings.value(key, defaultValue);
}

void ConfigStore::setValue(const QString& key, const QVariant& value)
{
  mSettings.setValue(key, value);
  mSettings.sync(); // save changes to disk
}
