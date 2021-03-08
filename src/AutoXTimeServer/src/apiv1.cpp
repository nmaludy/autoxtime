#include "apiv1.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>
#include <QUuid>
#include <QDebug>
#include <QtSql>
#include <Cutelyst/Plugins/Utils/Sql>

using namespace Cutelyst;

ApiV1::ApiV1(QObject *parent) : Controller(parent)
{
}

ApiV1::~ApiV1()
{
}

void ApiV1::drivers(Context *c)
{
    qDebug() << Q_FUNC_INFO;
}

void ApiV1::drivers_GET(Context *c)
{
    qDebug() << Q_FUNC_INFO;
    // QSettings s;
    // const QStringList uuids = s.childGroups();
    QSqlQuery query = CPreparedSqlQueryThreadForDB("SELECT * FROM driver", "AutoXTimeDB");
    if (query.exec()) {
        c->response()->setJsonArrayBody(QJsonArray::fromVariantList(Sql::queryToHashList(query)));
    }
}
