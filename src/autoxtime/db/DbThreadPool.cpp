#include <autoxtime/db/DbThreadPool.h>

#include <autoxtime/config/ConfigStore.h>

AUTOXTIME_DB_NAMESPACE_BEG

DbThreadPool::DbThreadPool()
    : mNumThreads(ConfigStore::valueCast<std::uint32_t>("db/num_threads", 1)),
      mpThreadPool(std::make_unique<QThreadPool>())
{
  mpThreadPool->setMaxThreadCount(mNumThreads);
  for (std::uint32_t i = 0; i < mNumThreads; ++i)
  {
    mpThreadPool->reserveThread();
  }
}

DbThreadPool& DbThreadPool::instance()
{
  static DbThreadPool inst;
  return inst;
}



AUTOXTIME_DB_NAMESPACE_END
2
