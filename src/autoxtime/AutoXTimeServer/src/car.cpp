#include "car.h"

using namespace Cutelyst;

Car::Car(QObject *parent) : Controller(parent)
{
}

Car::~Car()
{
}

void Car::index(Context *c)
{
    c->response()->body() = "Matched Controller::Car in Car.";
}

