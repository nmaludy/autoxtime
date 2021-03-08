#ifndef RUN_H
#define RUN_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

class Run : public Controller
{
    Q_OBJECT
public:
    explicit Run(QObject *parent = nullptr);
    ~Run();

    C_ATTR(index, :Path :AutoArgs)
    void index(Context *c);
};

#endif //RUN_H

