#include <autoxtime/ui/StandardItem.h>

AUTOXTIME_UI_NAMESPACE_BEG

StandardItem::StandardItem(int editRole)
    : QStandardItem(),
      mEditRole(editRole)
{}

StandardItem::~StandardItem()
{}

void StandardItem::setData(const QVariant& value, int role)
{
  // if the data is the same, don't set it
  // this avoids random writes to the database when the editor signifies something changed
  // but it really hasn't... it probably can't tell the difference
  if (value == data(role)) { return; }

  QStandardItem::setData(value, role);
  if (role == Qt::EditRole)
  {
    QStandardItem::setData(value, mEditRole);
  }
}

AUTOXTIME_UI_NAMESPACE_END
