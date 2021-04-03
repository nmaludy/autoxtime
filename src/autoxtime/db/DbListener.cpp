#include <autoxtime/db/DbListener.h>
#include <autoxtime/db/DbConnection.h>
#include <autoxtime/log/Log.h>

#include <iostream>

AUTOXTIME_DB_NAMESPACE_BEG

DbListener::DbListener(const QString& channel,
                       QObject* pParent)
    : QObject(pParent),
      mChannel(channel),
      mpConnection(new DbConnection("autoxtime::DbListener::" + channel, this))
{
  // hook up our listener before subscribing
  connect(mpConnection->database().driver(),
          qOverload<const QString&, QSqlDriver::NotificationSource , const QVariant&>(&QSqlDriver::notification),
          this,
          &DbListener::notification);
  // subscribe
  mpConnection->database().driver()->subscribeToNotification(channel);
}

void DbListener::notification(const QString& name,
                              QSqlDriver::NotificationSource source,
                              const QVariant& payload)
{
  // commented out because they are unused and causing build failures
  // QJsonDocument payload_doc = QJsonDocument::fromJson(payload.toString().toUtf8());
  // QJsonObject payload_json = payload_doc.object();
  // QJsonObject payload_data = payload_json.value("data").toObject();

  AXT_DEBUG << "Notification name=" << name
            << " payload=" << payload.toString();
}

AUTOXTIME_DB_NAMESPACE_END
