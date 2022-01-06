#ifndef AUTOXTIME_UTIL_COLORUTIL
#define AUTOXTIME_UTIL_COLORUTIL

#include <autoxtime/util/util.h>

#include <QColor>
#include <QString>

#include <string>
#include <unordered_map>

AUTOXTIME_UTIL_NAMESPACE_BEG

class ColorUtil
{
 public:
  static std::unordered_map<QString, QColor> COLOR_MAP;

  // converts a randm/crazy name into a color
  // examples:
  // - "Gray"
  // - "GREY"
  // - "silver"
  // - "dark red"
  static QColor nameToColor(const QString& name);
  static QColor nameToColor(const std::string& name);
};

AUTOXTIME_UTIL_NAMESPACE_END

#endif // AUTOXTIME_UTIL_COLORUTIL
