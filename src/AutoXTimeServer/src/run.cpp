#include "run.h"

using namespace Cutelyst;

Run::Run(QObject *parent) : Controller(parent)
{
}

Run::~Run()
{
}

void Run::index(Context *c)
{
    c->response()->body() = "Matched Controller::Run in Run.";
}

