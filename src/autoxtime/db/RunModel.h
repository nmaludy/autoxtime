#ifndef AUTOXTIME_DB_RUNMODEL
#define AUTOXTIME_DB_RUNMODEL

#include <autoxtime/db/db.h>
#include <autoxtime/db/BaseModelT.h>

namespace autoxtime { namespace proto { class Run; } }

AUTOXTIME_DB_NAMESPACE_BEG

class RunModel : public BaseModelT<autoxtime::proto::Run, RunModel>
{
  Q_OBJECT

 public:
  static const std::string TABLE;
  static const std::string PRIMARY_KEY;

  RunModel(QObject* pParent = nullptr);
  RunModel(std::shared_ptr<DbConnection> pConnection,
           QObject* pParent = nullptr);

  virtual void emitSignal(Signal signal,
                          const std::vector<std::shared_ptr<autoxtime::proto::Run> >& results);

 signals:
  // need the full namespace here so it matches the Q_DECLARE_METATYPE below
  void listResult(const std::vector<std::shared_ptr<autoxtime::proto::Run> >& protoList);
  void createResult(const std::vector<std::shared_ptr<autoxtime::proto::Run> >& protoList);
  void updateResult(const std::vector<std::shared_ptr<autoxtime::proto::Run> >& protoList);
  void findResult(const std::vector<std::shared_ptr<autoxtime::proto::Run> >& protoList);
};

AUTOXTIME_DB_NAMESPACE_END

// needed so we can use signals/slots with this type
Q_DECLARE_METATYPE(std::vector<std::shared_ptr<autoxtime::proto::Run>>)

#endif // AUTOXTIME_DB_RUNMODEL
