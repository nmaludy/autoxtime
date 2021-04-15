#include <autoxtime/db/DbThreadPool.h>

#include <autoxtime/config/ConfigStore.h>
#include <autoxtime/log/Log.h>
#include <autoxtime/db/DbConnection.h>

#include <QMutex>
#include <QWaitCondition>
#include <QSemaphore>

#include <memory>

AUTOXTIME_DB_NAMESPACE_BEG

DbThreadPool::DbThreadPool()
    : QThreadPool(),
      mNumThreads(ConfigStore::valueCast<std::uint32_t>("db/num_threads", 3))
{
  setMaxThreadCount(mNumThreads);
  // never expire threads, so the threads we start and run below are the ones
  // for the rest of the thread pool
  // this allows the database connections created to always be good
  setExpiryTimeout(-1);

  // initialize database connnection from main thread, this should setup the database
  // without a race condition
  DbConnection main_con;

  std::shared_ptr<QSemaphore> p_semaphore = std::make_unique<QSemaphore>();
  std::shared_ptr<QMutex> p_mutex = std::make_unique<QMutex>();
  std::shared_ptr<QWaitCondition> p_wait_condition = std::make_unique<QWaitCondition>();
  for (std::uint32_t i = 0; i < mNumThreads; ++i)
  {
    AXT_DEBUG << "Kicking off thread  " << i;
    start([=]() {
      AXT_DEBUG << "Started thread  " << i;
      p_semaphore->release(1);
      AXT_DEBUG << "Locking mutext thread " << i;
      p_mutex->lock();
      AXT_DEBUG << "Initializing database thread " << i;
      DbConnection con;
      AXT_DEBUG << "Waiting for condition thread " << i;
      p_wait_condition->wait(p_mutex.get());
      AXT_DEBUG << "condition woken up thread " << i;
      p_mutex->unlock();
    });
  }

  for (std::uint32_t i = 0; i < mNumThreads; ++i)
  {
    AXT_DEBUG << "Waiting for thread to start " << i;
    p_semaphore->acquire(1);
    AXT_DEBUG << "Thread started  " << i;
  }

  AXT_DEBUG << "Waiting for mutex thread main";
  p_mutex->lock();
  AXT_DEBUG << "Waking up all thread main";
  p_wait_condition->wakeAll();
  AXT_DEBUG << "unlocking main";
  p_mutex->unlock();
}

DbThreadPool* DbThreadPool::instance()
{
  static DbThreadPool inst;
  return &inst;
}

AUTOXTIME_DB_NAMESPACE_END
