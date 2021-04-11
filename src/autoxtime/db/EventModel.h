#ifndef AUTOXTIME_DB_EVENTMODEL
#define AUTOXTIME_DB_EVENTMODEL

#include <autoxtime/db/db.h>
#include <autoxtime/db/BaseModelT.h>

namespace autoxtime { namespace proto { class Event; } }

AUTOXTIME_DB_NAMESPACE_BEG

class EventModel : public BaseModelT<autoxtime::proto::Event, EventModel>
{
  Q_OBJECT

 public:
  static const std::string TABLE;
  static const std::string PRIMARY_KEY;

  explicit EventModel(QObject* pParent = nullptr);
  explicit EventModel(std::shared_ptr<DbConnection> pConnection,
                      QObject* pParent = nullptr);

  virtual void emitSignal(Signal signal,
                          const std::vector<std::shared_ptr<autoxtime::proto::Event> >& results);

 signals:
  // need the full namespace here so it matches the Q_DECLARE_METATYPE below
  void listResult(const std::vector<std::shared_ptr<autoxtime::proto::Event> >& protoList);
  void createResult(const std::vector<std::shared_ptr<autoxtime::proto::Event> >& protoList);
  void updateResult(const std::vector<std::shared_ptr<autoxtime::proto::Event> >& protoList);
  void findResult(const std::vector<std::shared_ptr<autoxtime::proto::Event> >& protoList);
};

AUTOXTIME_DB_NAMESPACE_END

// needed so we can use signals/slots with this type
Q_DECLARE_METATYPE(std::vector<std::shared_ptr<autoxtime::proto::Event>>)

#endif // AUTOXTIME_DB_EVENT
