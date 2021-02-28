#ifndef AUTOXTIMESERVER_H
#define AUTOXTIMESERVER_H

#include <Cutelyst/Application>

using namespace Cutelyst;

class AutoXTimeServer : public Application
{
    Q_OBJECT
    CUTELYST_APPLICATION(IID "AutoXTimeServer")
public:
    Q_INVOKABLE explicit AutoXTimeServer(QObject *parent = nullptr);
    ~AutoXTimeServer();

    virtual bool postFork() override;

    bool init();
};

#endif //AUTOXTIMESERVER_H

