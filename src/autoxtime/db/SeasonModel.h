#ifndef AUTOXTIME_DB_SEASONMODEL
#define AUTOXTIME_DB_SEASONMODEL

#include <autoxtime/db/db.h>
#include <autoxtime/db/BaseModelT.h>

namespace autoxtime { namespace proto { class Season; } }

AUTOXTIME_DB_NAMESPACE_BEG

class SeasonModel : public BaseModelT<autoxtime::proto::Season, SeasonModel>
{
  Q_OBJECT

 public:
  static const std::string TABLE;
  static const std::string PRIMARY_KEY;

  explicit SeasonModel(QObject* pParent = nullptr);
  explicit SeasonModel(std::shared_ptr<DbConnection> pConnection,
                             QObject* pParent = nullptr);

  virtual void emitSignal(Signal signal,
                          const std::vector<std::shared_ptr<autoxtime::proto::Season> >& results);

 signals:
  // need the full namespace here so it matches the Q_DECLARE_METATYPE below
  void listResult(const std::vector<std::shared_ptr<autoxtime::proto::Season> >& protoList);
  void createResult(const std::vector<std::shared_ptr<autoxtime::proto::Season> >& protoList);
  void updateResult(const std::vector<std::shared_ptr<autoxtime::proto::Season> >& protoList);
  void findResult(const std::vector<std::shared_ptr<autoxtime::proto::Season> >& protoList);
};

AUTOXTIME_DB_NAMESPACE_END

// needed so we can use signals/slots with this type
Q_DECLARE_METATYPE(std::vector<std::shared_ptr<autoxtime::proto::Season>>)

#endif // AUTOXTIME_DB_SEASON
