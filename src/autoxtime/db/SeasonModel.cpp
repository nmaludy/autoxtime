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
    : BaseModel(SeasonModel::TABLE,
                SeasonModel::PRIMARY_KEY,
                autoxtime::proto::Season::GetDescriptor(),
                autoxtime::proto::Season::GetReflection(),
                pConnection,
                pParent)
{
  qRegisterMetaType<SeasonModel::ProtoPtrVec>();
}

SeasonModel::ProtoPtrVec SeasonModel::list()
{
  return BaseModel::listT<SeasonModel::Proto>();
}

QFuture<SeasonModel::ProtoPtrVec> SeasonModel::listAsync()
{
  return QtConcurrent::run([=]() {
    // Code in this block will run in another thread

    // TODO, can we avoid createing new models for each query?
    // right now we are doing this for thread safety, probably for the better (honestly)
    std::unique_ptr<SeasonModel> p_model = std::make_unique<SeasonModel>();
    SeasonModel::ProtoPtrVec results = p_model->list();
    emit listResult(results);
    return results;
  });
}

SeasonModel::ProtoPtrVec SeasonModel
::create(const SeasonModel::Proto& season)
{
  return BaseModel::createT(season);
}

SeasonModel::ProtoPtrVec SeasonModel
::update(const SeasonModel::Proto& season)
{
  return BaseModel::updateT(season);
}

SeasonModel::ProtoPtrVec SeasonModel::find(const SeasonModel::Proto& prototype)
{
  return BaseModel::findT<SeasonModel::Proto>(prototype);
}

SeasonModel::ProtoPtrVec SeasonModel::findById(std::int64_t id)
{
  return BaseModel::findByIdT<SeasonModel::Proto>(id);
}

AUTOXTIME_DB_NAMESPACE_END
