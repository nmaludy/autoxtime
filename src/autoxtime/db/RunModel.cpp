#include <autoxtime/db/RunModel.h>

#include <autoxtime/proto/run.pb.h>

AUTOXTIME_DB_NAMESPACE_BEG

const std::string RunModel::TABLE = "run";
const std::string RunModel::PRIMARY_KEY = "run_id";

RunModel::RunModel(QObject* pParent)
    : RunModel(std::shared_ptr<DbConnection>(), pParent)
{}

RunModel::RunModel(std::shared_ptr<DbConnection> pConnection,
                   QObject* pParent)
    : BaseModelT(RunModel::TABLE,
                 RunModel::PRIMARY_KEY,
                 autoxtime::proto::Run::GetDescriptor(),
                 autoxtime::proto::Run::GetReflection(),
                 pConnection,
                 pParent)
{}

void RunModel
::emitSignal(RunModel::Signal signal,
             const std::vector<std::shared_ptr<autoxtime::proto::Run> >& protoList)
{
  switch (signal)
  {
    case SIGNAL_LIST_RESULT:
      emit listResult(protoList);
      break;
    case SIGNAL_CREATE_RESULT:
      emit createResult(protoList);
      break;
    case SIGNAL_UPDATE_RESULT:
      emit updateResult(protoList);
      break;
    case SIGNAL_FIND_RESULT:
      emit findResult(protoList);
      break;
  }
}

AUTOXTIME_DB_NAMESPACE_END
