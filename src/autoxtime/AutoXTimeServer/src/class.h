#ifndef CLASS_H
#define CLASS_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

class Class : public Controller
{
    Q_OBJECT
public:
    explicit Class(QObject *parent = nullptr);
    ~Class();

    C_ATTR(index, :Path :AutoArgs)
    void index(Context *c);
};

#endif //CLASS_H

