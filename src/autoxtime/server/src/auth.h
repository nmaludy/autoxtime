#ifndef AUTH_H
#define AUTH_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

class Auth : public Controller
{
    Q_OBJECT
public:
    explicit Auth(QObject *parent = nullptr);
    ~Auth();

    C_ATTR(index, :Path :AutoArgs)
    void index(Context *c);
};

#endif //AUTH_H

