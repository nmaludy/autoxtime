#ifndef AUTOXTIME_DB_DBTHREADPOOL
#define AUTOXTIME_DB_DBTHREADPOOL

#include <autoxtime/db/db.h>

#include <cstdint>
#include <memory>

class QThreadPool;

AUTOXTIME_DB_NAMESPACE_BEG

class DbThreadPool
{
 public:
  static DbThreadPool& instance();

 private:
  DbThreadPool();

  std::uint32_t mNumThreads;
  std::unique_ptr<QThreadPool> mpThreadPool;
};

AUTOXTIME_DB_NAMESPACE_END

#endif // AUTOXTIME_DB_DBCONNECTION
