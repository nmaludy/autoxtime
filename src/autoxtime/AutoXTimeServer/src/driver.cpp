#include "driver.h"

#include <QtSql>
#include <Cutelyst/Plugins/Utils/Sql>

using namespace Cutelyst;

Driver::Driver(QObject *parent) : Controller(parent)
{
}

Driver::~Driver()
{
}

void Driver::base(Context *c)
{
    // Print a message to the debug log
    qDebug("*** INSIDE BASE METHOD ***");
}

void Driver::index(Context *c)
{
    c->response()->redirect(c->uriFor(CActionFor("list")));
}

void Driver::list(Context *c)
{
    // c is the Cutelyst::Context that's used to 'glue together'
    // the various components that make up the application

    // Retrieve all of the book records as book model objects and store in the
    // stash where they can be accessed by the Grantlee template
    QSqlQuery query = CPreparedSqlQueryThreadForDB("SELECT * FROM driver", "AutoXTimeDB");
    if (query.exec()) {
        c->setStash("drivers", Sql::queryToHashList(query));
    }
    // Set the Grantlee template to use. You will almost always want to do this
    // in your action methods (action methods respond to user input in
    // your controllers).
    c->setStash("template", "driver/list.html");
}

void Driver::form_create(Context *c)
{
    // Set the Grantlee Template to use
    c->setStash("template", "driver/form_create.html");
}

void Driver::form_create_do(Context *c)
{
    // Retrieve the values from the form
    QString first_name = c->request()->bodyParam("first_name");
    QString last_name = c->request()->bodyParam("last_name");
    QString email = c->request()->bodyParam("email");
    QString phone_number = c->request()->bodyParam("phone_number");
    QString msr_id = c->request()->bodyParam("msr_id");
    QString scca_id = c->request()->bodyParam("scca_id");

    // Insert the book into it's table
    QSqlQuery query = CPreparedSqlQueryThreadForDB("INSERT INTO driver (first_name, last_name, email, phone_number, msr_id, scca_id) VALUES (:first_name, :last_name, :email, :phone_number, :msr_id, :scca_id)", "AutoXTimeDB");
    query.bindValue(":first_name", first_name);
    query.bindValue(":last_name", last_name);
    query.bindValue(":email", email);
    query.bindValue(":phone_number", phone_number);
    query.bindValue(":msr_id", msr_id);
    query.bindValue(":scca_id", scca_id);

    bool error = true;
    if (query.exec()) {
        error = false;
    }

    // On error show the last one
    if (error) {
        c->stash()["error_msg"] = query.lastError().text();
    }

    // Assign the Book object to the stash for display and set template
    c->stash({
       {"driver", QVariant::fromValue(c->request()->bodyParams())},
       {"template", "driver/driver_get.html"}
    });
}

void Driver::object(Context *c, const QString &id)
{
    // Find the object on the database
    QSqlQuery query = CPreparedSqlQueryThreadForDB("SELECT * FROM driver WHERE driver_id = :id", "AutoXTimeDB");
    query.bindValue(":id", id);
    if (query.exec()) {
        c->setStash("object", Sql::queryToHashObject(query));
    } else {
        // You would probably want to do something like this in a real app:
        // c->detach("/error_404");
    }
    qDebug() << "*** INSIDE OBJECT METHOD for obj id=" << id << " ***";
}

void Driver::driver_get(Context *c)
{
    QVariantHash driver = c->stash("object").toHash();
    QString statusMsg;

    c->stash({
       {"driver", driver},
       {"template", "driver/driver_get.html"}
    });
}

void Driver::form_update_do(Context *c)
{
    // Retrieve the values from the form
    QString driver_id = c->request()->bodyParam("driver_id");
    QString first_name = c->request()->bodyParam("first_name");
    QString last_name = c->request()->bodyParam("last_name");
    QString email = c->request()->bodyParam("email");
    QString phone_number = c->request()->bodyParam("phone_number");
    QString msr_id = c->request()->bodyParam("msr_id");
    QString scca_id = c->request()->bodyParam("scca_id");
    QString Update = c->request()->bodyParam("Update");
    QString Delete = c->request()->bodyParam("Delete");

    // Insert the book into it's table
    QSqlQuery query;
    if (Update == "Update") {
      query = CPreparedSqlQueryThreadForDB("UPDATE driver SET (first_name, last_name, email, phone_number, msr_id, scca_id) = (:first_name, :last_name, :email, :phone_number, :msr_id, :scca_id) WHERE driver.driver_id = :driver_id", "AutoXTimeDB");
      query.bindValue(":first_name", first_name);
      query.bindValue(":last_name", last_name);
      query.bindValue(":email", email);
      query.bindValue(":phone_number", phone_number);
      query.bindValue(":msr_id", msr_id);
      query.bindValue(":scca_id", scca_id);
      query.bindValue(":driver_id", driver_id);
    }

    if (Delete == "Delete") {
      query = CPreparedSqlQueryThreadForDB("DELETE FROM driver WHERE driver_id = :driver_id", "AutoXTimeDB");
      query.bindValue(":driver_id", driver_id);
    }

    QString statusMsg;
    if (query.exec()) {
      if (Update == "Update") {
        statusMsg = "Driver Updated.";
      }
      if (Delete == "Delete") {
        statusMsg = "Driver deleted.";
      }
    } else {
        // Set an error message to be displayed at the top of the view
        statusMsg = query.lastError().text();
    }

    c->response()->redirect(c->uriFor(CActionFor("list"), ParamsMultiMap{
                              {"status_msg", statusMsg }
                          }));
}

void Driver::find(Context *c)
{
    // Set the Grantlee Template to use
    c->setStash("template", "driver/find.html");
}

void Driver::find_do(Context *c)
{
    // Retrieve the values from the form
    QString first_name = QString("\%%1\%").arg(c->request()->bodyParam("first_name"));
    QString last_name = QString("\%%1\%").arg(c->request()->bodyParam("last_name"));
    QString email = QString("\%%1\%").arg(c->request()->bodyParam("email"));
    QString phone_number = QString("\%%1\%").arg(c->request()->bodyParam("phone_number"));
    QString msr_id = c->request()->bodyParam("msr_id", "");
    QString scca_id = c->request()->bodyParam("scca_id", "");

    QSqlQuery query = CPreparedSqlQueryThreadForDB("SELECT * FROM driver WHERE first_name ILIKE :first_name OR last_name ILIKE :last_name OR email ILIKE :email OR phone_number ILIKE :phone_number OR msr_id = :msr_id OR scca_id = :scca_id", "AutoXTimeDB");

    query.bindValue(":first_name", first_name);
    query.bindValue(":last_name", last_name);
    query.bindValue(":email", email);
    query.bindValue(":phone_number", phone_number);
    query.bindValue(":msr_id", msr_id);
    query.bindValue(":scca_id", scca_id);

    QString statusMsg;
    if (query.exec()) {
        int rows = query.size();
        if (rows == 1) {
            c->stash({
                {"driver", Sql::queryToHashObject(query)},
                {"template", "driver/driver_get.html"}
            });
        } else if (rows <= 0) {
            c->response()->redirect(c->uriFor(CActionFor("list"), ParamsMultiMap{
                              {"status_msg", "Could not find Driver with the information provided!" }
                          }));
        } else {
            c->setStash("drivers", Sql::queryToHashList(query));
            c->response()->redirect(c->uriFor(CActionFor("list")));
        }
    } else {
        // Set an error message to be displayed at the top of the view
        statusMsg = query.lastError().text();
        c->response()->redirect(c->uriFor(CActionFor("list"), ParamsMultiMap{
                              {"status_msg", statusMsg }
                          }));
    }
}