#ifndef AUTOXTIME_UI_STANDARDITEM
#define AUTOXTIME_UI_STANDARDITEM

#include <autoxtime/ui/ui.h>
#include <QStandardItem>

class QVariant;

AUTOXTIME_UI_NAMESPACE_BEG

class StandardItem : public QStandardItem
{
 public:
  enum Role
  {
    ROLE_EDIT = Qt::UserRole,
  };

  StandardItem(int editRole = ROLE_EDIT);
  virtual ~StandardItem();
  virtual void setData(const QVariant& value, int role);

 private:
  int mEditRole;
};

AUTOXTIME_UI_NAMESPACE_END

#endif // AUTOXTIME_UI_STANDARDITEM
