#include <autoxtime/util/ColorUtil.h>

#include <autoxtime/log/Log.h>

#include <QRegularExpression>

AUTOXTIME_UTIL_NAMESPACE_BEG

// TODO make these configurable
std::unordered_map<QString, QColor> ColorUtil::COLOR_MAP = {
  {"aluminum", Qt::gray},
  {"beige", QColor(0xF5F5DC)},
  {"black", Qt::black},
  {"blue", Qt::blue},
  {"cyan", Qt::cyan},
  {"gray", Qt::gray},
  {"grey", Qt::gray},
  {"green", Qt::green},
  {"lime", Qt::green},
  {"magenta", Qt::magenta},
  {"maroon", QColor(0x800000)},
  {"orange", QColor(0xFFA500)},
  {"pewter", Qt::gray},
  {"red", Qt::red},
  {"silver", Qt::lightGray},
  {"teal", QColor(0x008080)},
  {"white", Qt::white},
  {"yellow", Qt::yellow},
};

QColor ColorUtil::nameToColor(const QString& name)
{
  QString lower = name.toLower();
  QStringList sl = lower.split(QRegularExpression("[^[:alpha:]]+"));
  for (QString s : sl)
  {
    std::unordered_map<QString, QColor>::iterator iter = COLOR_MAP.find(s);
    if (iter != COLOR_MAP.end())
    {
      return iter->second;
    }
  }
  return QColor();
}

QColor ColorUtil::nameToColor(const std::string& name)
{
  return nameToColor(QString::fromStdString(name));
}

AUTOXTIME_UTIL_NAMESPACE_END
