#include "autoxtimeserver.h"
#include "root.h"
// #include "auth.h"
// #include "car.h"
// #include "class.h"
#include "driver.h"
// #include "run.h"
#include "apiv1.h"

#include <Cutelyst/Plugins/StaticSimple/staticsimple.h>
#include <Cutelyst/Plugins/View/Grantlee/grantleeview.h>
#include <QtSql>
#include <Cutelyst/Plugins/Utils/Sql>
// #include <Cutelyst/Plugins/Session/Session>
// #include <Cutelyst/Plugins/Authentication/authentication.h>
// #include <Cutelyst/Plugins/Authentication/credentialpassword.h>

using namespace Cutelyst;

AutoXTimeServer::AutoXTimeServer(QObject *parent) : Application(parent)
{
}

AutoXTimeServer::~AutoXTimeServer()
{
}

bool AutoXTimeServer::init()
{
    new Root(this);
    new StaticSimple(this);
    // new Auth(this);
    // new Car(this);
    // new Class(this);
    new Driver(this);
    new ApiV1(this);
    // new Run(this);

    // auto auth = new Authentication(this);
    // auto credential = new CredentialPassword;
    // credential->setPasswordType(CredentialPassword::Clear);

    // auth->addRealm(new AuthStoreSql, credential);

    auto view = new GrantleeView(this);
    view->setIncludePaths({ pathTo("root/src") });
    view->setWrapper("wrapper.html");

    return true;
}

bool AutoXTimeServer::postFork()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", Sql::databaseNameThread("AutoXTimeDB"));
    db.setDatabaseName("autoxtime");
    db.setUserName("postgres");
    db.setPassword("password");
    db.setHostName("localhost");
    db.setPort(5432);
    if (!db.open()) {
        qCritical() << "Failed to open database:" << db.lastError().text();
        return false;
    }
    return true;
}

