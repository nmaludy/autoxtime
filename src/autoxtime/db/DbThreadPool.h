#ifndef AUTOXTIME_DB_DBTHREADPOOL
#define AUTOXTIME_DB_DBTHREADPOOL

#include <autoxtime/db/db.h>

// qt
#include <QFuture>
#include <QThreadPool>

// std
#include <cstdint>
#include <functional>
#include <memory>

AUTOXTIME_DB_NAMESPACE_BEG

class DbThreadPool : public QThreadPool
{
 public:
  static DbThreadPool* instance();

 private:
  DbThreadPool();

  std::uint32_t mNumThreads;
};

AUTOXTIME_DB_NAMESPACE_END

#endif // AUTOXTIME_DB_DBCONNECTION
