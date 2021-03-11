#ifndef DRIVER_H
#define DRIVER_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

class Driver : public Controller
{
    Q_OBJECT
public:
    explicit Driver(QObject *parent = nullptr);
    ~Driver();

    C_ATTR(index, :Path :AutoArgs)
    void index(Context *c);

    C_ATTR(base, :Chained("/") :PathPart("driver") :CaptureArgs(0))
    void base(Context *c);

    C_ATTR(list, :Chained("base") :PathPart("list") :Args(0))
    void list(Context *c);

    // C_ATTR(url_create, :Chained("base") :PathPart("url_create") :Args(3))
    // void url_create(Context *c, const QString &title, const QString &rating, const QString &authorId);

    C_ATTR(form_create, :Chained("base") :PathPart("form_create") :Args(0))
    void form_create(Context *c);

    C_ATTR(form_create_do, :Chained("base") :PathPart("form_create_do") :Args(0))
    void form_create_do(Context *c);

    C_ATTR(object, :Chained("/") :PathPart("driver") :CaptureArgs(1))
    void object(Context *c, const QString &id);

    C_ATTR(driver_get, :Chained("object") :PathPart("driver_get") :Args(0))
    void driver_get(Context *c);

    C_ATTR(form_update_do, :Chained("object") :PathPart("form_update_do") :Args(0))
    void form_update_do(Context *c);

    C_ATTR(find, :Chained("base") :PathPart("find") :Args(0))
    void find(Context *c);

    C_ATTR(find_do, :Chained("base") :PathPart("find_do") :Args(0))
    void find_do(Context *c);
};

#endif //DRIVER_H

