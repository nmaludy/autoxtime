#ifndef AUTOXTIME_TRANSPORT_ITRANSPORT
#define AUTOXTIME_TRANSPORT_ITRANSPORT

#include <autoxtime/autoxtime.h>
#include <QByteArray>
#include <QList>
#include <QObject>

AUTOXTIME_NAMESPACE_BEG

class ITransport : public QObject
{
  Q_OBJECT

 public:
  explicit ITransport(QObject* pParent = nullptr);

 signals:
  void dataRead(const QByteArray& data);
};

AUTOXTIME_NAMESPACE_END

#endif // AUTOXTIME_TRANSPORT_ITRANSPORT
