#ifndef AUTOXTIME_DB_DRIVER
#define AUTOXTIME_DB_DRIVER

#include <autoxtime/autoxtime.h>
#include <QtSql/QtSql>
#include <QTimer>

AUTOXTIME_NAMESPACE_BEG

class Driver : public QObject
{
  Q_OBJECT;

 public:
  int id;
  QString first_name;
  QString last_name;
  QString email;
  QString phone_number;
  int msr_id;
  int scca_id;
  int workreq;
  int workskill;

  static std::vector<Driver> list();
  static bool create(const Driver& driver);
  static bool update(const Driver& driver);
  static bool findById(int id);
};

AUTOXTIME_NAMESPACE_END

#endif // AUTOXTIME_DB_DRIVER
