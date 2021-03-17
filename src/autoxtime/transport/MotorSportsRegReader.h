#ifndef AUTOXTIME_TRANSPORT_MOTORSPORTSREGREADER
#define AUTOXTIME_TRANSPORT_MOTORSPORTSREGREADER

#include <autoxtime/autoxtime.h>
#include <autoxtime/transport/ITransport.h>

AUTOXTIME_NAMESPACE_BEG

class MotorSportsRegReader : public ITransport
{
  Q_OBJECT
 public:
  explicit MotorSportsRegReader(QObject* pParent = nullptr);

  void readFile(const QString& fileName);
};

AUTOXTIME_NAMESPACE_END

#endif // AUTOXTIME_TRANSPORT_SERIALPORTASYNCREADER
