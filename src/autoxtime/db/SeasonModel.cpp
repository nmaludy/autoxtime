#include <autoxtime/db/SeasonModel.h>

#include <autoxtime/db/DbConnection.h>
#include <autoxtime/proto/season.pb.h>

#include <QtDebug>
#include <QtConcurrent/QtConcurrent>

AUTOXTIME_DB_NAMESPACE_BEG

const std::string SeasonModel::TABLE = "season";
const std::string SeasonModel::PRIMARY_KEY = "season_id";

SeasonModel::SeasonModel(QObject* pParent)
    : SeasonModel(std::shared_ptr<DbConnection>(), pParent)
{}

SeasonModel::SeasonModel(std::shared_ptr<DbConnection> pConnection,
                         QObject* pParent)
    : BaseModelT(SeasonModel::TABLE,
                 SeasonModel::PRIMARY_KEY,
                 autoxtime::proto::Season::GetDescriptor(),
                 autoxtime::proto::Season::GetReflection(),
                 pConnection,
                 pParent)
{}

void SeasonModel
::emitSignal(SeasonModel::Signal signal,
             const std::vector<std::shared_ptr<autoxtime::proto::Season> >& protoList)
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
