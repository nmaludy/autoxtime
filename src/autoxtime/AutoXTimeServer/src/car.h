#ifndef CAR_H
#define CAR_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

class Car : public Controller
{
    Q_OBJECT
public:
    explicit Car(QObject *parent = nullptr);
    ~Car();

    C_ATTR(index, :Path :AutoArgs)
    void index(Context *c);
};

#endif //CAR_H

