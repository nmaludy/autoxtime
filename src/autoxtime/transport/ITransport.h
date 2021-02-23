#ifndef AUTOXTIME_TRANSPORT_ITRANSPORT
#define AUTOXTIME_TRANSPORT_ITRANSPORT

#include <QByteArray>
#include <QList>
#include <QObject>

class ITransport : public QObject
{
  Q_OBJECT

 public:
  ITransport(QObject* pParent);

 signals:
  void dataRead(const QByteArray& data);
};

#endif // AUTOXTIME_TRANSPORT_ITRANSPORT
