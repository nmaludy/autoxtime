#include "auth.h"

using namespace Cutelyst;


Auth::Auth(QObject *parent) : Controller(parent)
{
}

Auth::~Auth()
{
}

void Auth::index(Context *c)
{
    c->response()->body() = "Matched Controller::Auth in Auth.";
}
