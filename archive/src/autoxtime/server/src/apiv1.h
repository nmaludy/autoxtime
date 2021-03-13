#ifndef APIV1_H
#define APIV1_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

class ApiV1 : public Controller
{
    Q_OBJECT
public:
    explicit ApiV1(QObject *parent = nullptr);
    ~ApiV1();

    C_ATTR(drivers, :Local :AutoArgs :ActionClass(REST))
    void drivers(Context *c);

    C_ATTR(drivers_GET, :Private)
    void drivers_GET(Context *c);

};

#endif //APIV1_H

