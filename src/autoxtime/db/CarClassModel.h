#ifndef AUTOXTIME_DB_CARCLASSMODEL
#define AUTOXTIME_DB_CARCLASSMODEL

#include <autoxtime/db/db.h>
#include <autoxtime/db/BaseModelT.h>

namespace autoxtime { namespace proto { class CarClass; } }

AUTOXTIME_DB_NAMESPACE_BEG

class CarClassModel : public BaseModelT<autoxtime::proto::CarClass, CarClassModel>
{
  Q_OBJECT

 public:
  static const std::string TABLE;
  static const std::string PRIMARY_KEY;

  CarClassModel(QObject* pParent = nullptr);
  CarClassModel(std::shared_ptr<DbConnection> pConnection,
                QObject* pParent = nullptr);

  // creates CarClass if doesn't exist, order of lookup is:
  // - name + subclass_name
  std::vector<std::shared_ptr<autoxtime::proto::CarClass> > createIfNotExists(const autoxtime::proto::CarClass& carClass);

  virtual void emitSignal(Signal signal,
                          const std::vector<std::shared_ptr<autoxtime::proto::CarClass> >& results);

 signals:
  // need the full namespace here so it matches the Q_DECLARE_METATYPE below
  void listResult(const std::vector<std::shared_ptr<autoxtime::proto::CarClass> >& protoList);
  void createResult(const std::vector<std::shared_ptr<autoxtime::proto::CarClass> >& protoList);
  void updateResult(const std::vector<std::shared_ptr<autoxtime::proto::CarClass> >& protoList);
  void findResult(const std::vector<std::shared_ptr<autoxtime::proto::CarClass> >& protoList);
};

AUTOXTIME_DB_NAMESPACE_END

// needed so we can use signals/slots with this type
Q_DECLARE_METATYPE(std::vector<std::shared_ptr<autoxtime::proto::CarClass>>)

#endif // AUTOXTIME_DB_CARCLASS
