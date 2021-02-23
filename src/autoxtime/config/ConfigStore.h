#ifndef AUTOXTIME_CONFIG_CONFIGSTORE
#define AUTOXTIME_CONFIG_CONFIGSTORE

#include <QObject>
#include <QSettings>

class ConfigStore : public QObject
{
  Q_OBJECT

 public:
  static ConfigStore& instance(const QString& filePath = QString());

  const QString& filePath() const;

  QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;
  void setValue(const QString& key, const QVariant& value);
      
 private:
  ConfigStore(const QString& filePath, QObject* pParent = nullptr);

  QString mFilePath;
  QSettings mSettings;
};

#endif // AUTOXTIME_CONFIG_CONFIGSTORE
