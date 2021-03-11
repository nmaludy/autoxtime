#ifndef AUTOXTIME_DB_DRIVER
#define AUTOXTIME_DB_DRIVER

#include <autoxtime/autoxtime.h>
#include <autoxtime/proto/driver.pb.h>
#include <QtSql/QtSql>
#include <QTimer>

AUTOXTIME_NAMESPACE_BEG

class Driver : public QObject
{
  Q_OBJECT;

 public:

  static std::vector<autoxtime::proto::Driver> list();
  static bool create(const Driver& driver);
  static bool update(const Driver& driver);
  static bool findById(int id);
};

AUTOXTIME_NAMESPACE_END

#endif // AUTOXTIME_DB_DRIVER
