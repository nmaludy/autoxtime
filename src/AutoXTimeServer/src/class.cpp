#include "class.h"

using namespace Cutelyst;

Class::Class(QObject *parent) : Controller(parent)
{
}

Class::~Class()
{
}

void Class::index(Context *c)
{
    c->response()->body() = "Matched Controller::Class in Class.";
}

