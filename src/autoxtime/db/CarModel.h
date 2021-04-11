#ifndef AUTOXTIME_DB_CARMODEL
#define AUTOXTIME_DB_CARMODEL

#include <autoxtime/db/db.h>
#include <autoxtime/db/BaseModelT.h>

namespace autoxtime { namespace proto { class Car; } }

AUTOXTIME_DB_NAMESPACE_BEG

class CarModel : public BaseModelT<autoxtime::proto::Car, CarModel>
{
  Q_OBJECT

 public:
  static const std::string TABLE;
  static const std::string PRIMARY_KEY;

  CarModel(QObject* pParent = nullptr);
  CarModel(std::shared_ptr<DbConnection> pConnection,
           QObject* pParent = nullptr);

  // creates Car if doesn't exist, order of lookup is:
  // - model + class_id + driver_id + color (optional)
  std::vector<std::shared_ptr<autoxtime::proto::Car> > createIfNotExists(const autoxtime::proto::Car& car);

  virtual void emitSignal(Signal signal,
                          const std::vector<std::shared_ptr<autoxtime::proto::Car> >& results);

 signals:
  // need the full namespace here so it matches the Q_DECLARE_METATYPE below
  void listResult(const std::vector<std::shared_ptr<autoxtime::proto::Car> >& protoList);
  void createResult(const std::vector<std::shared_ptr<autoxtime::proto::Car> >& protoList);
  void updateResult(const std::vector<std::shared_ptr<autoxtime::proto::Car> >& protoList);
  void findResult(const std::vector<std::shared_ptr<autoxtime::proto::Car> >& protoList);
};

AUTOXTIME_DB_NAMESPACE_END

// needed so we can use signals/slots with this type
Q_DECLARE_METATYPE(std::vector<std::shared_ptr<autoxtime::proto::Car>>)

#endif // AUTOXTIME_DB_CAR
