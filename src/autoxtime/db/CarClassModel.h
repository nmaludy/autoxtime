#ifndef AUTOXTIME_DB_CARCLASSMODEL
#define AUTOXTIME_DB_CARCLASSMODEL

#include <autoxtime/db/db.h>
#include <autoxtime/db/BaseModel.h>

namespace autoxtime::proto { class CarClass; }

AUTOXTIME_DB_NAMESPACE_BEG

class CarClassModel : public BaseModel
{
  Q_OBJECT;

 public:
  static const std::string TABLE;
  static const std::string PRIMARY_KEY;

  CarClassModel(QObject* pParent = nullptr);
  CarClassModel(std::shared_ptr<DbConnection> pConnection,
                QObject* pParent = nullptr);

  std::vector<std::shared_ptr<autoxtime::proto::CarClass> > list();
  std::vector<std::shared_ptr<autoxtime::proto::CarClass> > create(const autoxtime::proto::CarClass& carClass);
  std::vector<std::shared_ptr<autoxtime::proto::CarClass> > update(const autoxtime::proto::CarClass& carClass);

  std::vector<std::shared_ptr<autoxtime::proto::CarClass> > find(const autoxtime::proto::CarClass& prototype);
  std::vector<std::shared_ptr<autoxtime::proto::CarClass> > findById(std::int64_t id);

  // creates CarClass if doesn't exist, order of lookup is:
  // - name + subclass_name
  std::vector<std::shared_ptr<autoxtime::proto::CarClass> > createIfNotExists(const autoxtime::proto::CarClass& carClass);
};

AUTOXTIME_DB_NAMESPACE_END

#endif // AUTOXTIME_DB_CARCLASS
