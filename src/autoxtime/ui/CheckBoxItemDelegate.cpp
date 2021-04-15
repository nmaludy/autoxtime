#include <autoxtime/ui/CheckBoxItemDelegate.h>

#include <autoxtime/log/Log.h>

#include <QApplication>
#include <QCheckBox>

AUTOXTIME_UI_NAMESPACE_BEG

CheckBoxItemDelegate::CheckBoxItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{}

void CheckBoxItemDelegate::commitAndCloseEditor()
{
  QCheckBox* editor = dynamic_cast<QCheckBox*>(sender());
  emit commitData(editor);
  emit closeEditor(editor);
}

QWidget* CheckBoxItemDelegate::createEditor(QWidget* parent,
                                            const QStyleOptionViewItem& option,
                                            const QModelIndex& index) const
{
  //create the checkbox editor
  QCheckBox* cb = new QCheckBox(parent);
  connect(cb,   &QCheckBox::toggled,
          this, &CheckBoxItemDelegate::commitAndCloseEditor);
  return cb;
}

void CheckBoxItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
  //set if checked or not
  QCheckBox* cb = dynamic_cast<QCheckBox*>(editor);
  cb->setCheckState(indexCheckState(index));
}

void CheckBoxItemDelegate::setModelData(QWidget* editor,
                                        QAbstractItemModel* model,
                                        const QModelIndex& index) const
{
  QCheckBox* cb = dynamic_cast<QCheckBox*>(editor);
  model->setData(index, cb->checkState(), Qt::CheckStateRole);
}

bool CheckBoxItemDelegate::editorEvent(QEvent* event,
                                       QAbstractItemModel* model,
                                       const QStyleOptionViewItem& option,
                                       const QModelIndex& index)
{
  if (event->type() == QEvent::MouseButtonRelease)
  {
    Qt::CheckState data = indexCheckState(index);
    // invert checkbox state
    Qt::CheckState inverted = (data == Qt::Checked) ? Qt::Unchecked : Qt::Checked;
    model->setData(index, inverted, Qt::CheckStateRole);
    return true;
  }
  return QStyledItemDelegate::editorEvent(event, model, option, index);
}

void CheckBoxItemDelegate::paint(QPainter* painter,
                                 const QStyleOptionViewItem& option,
                                 const QModelIndex& index) const
{
  //retrieve data
  Qt::CheckState data = indexCheckState(index);

  //create CheckBox style
  QStyleOptionButton checkboxstyle;
  QRect checkbox_rect = QApplication::style()->subElementRect(QStyle::SE_CheckBoxIndicator,
                                                              &checkboxstyle);

  //center
  checkboxstyle.rect = option.rect;
  checkboxstyle.rect.setLeft(option.rect.x() +
                             option.rect.width()/2 - checkbox_rect.width()/2);
  //checked or not checked
  if (data == Qt::Checked)
  {
    checkboxstyle.state = QStyle::State_On | QStyle::State_Enabled;
  }
  else
  {
    checkboxstyle.state = QStyle::State_Off | QStyle::State_Enabled;
  }

  //done! we can draw!
  QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkboxstyle, painter);
}

void CheckBoxItemDelegate::updateEditorGeometry(QWidget* editor,
                                                const QStyleOptionViewItem& option,
                                                const QModelIndex& index) const
{
  Q_UNUSED(index);
  QStyleOptionButton checkboxstyle;
  QRect checkbox_rect = QApplication::style()->subElementRect(QStyle::SE_CheckBoxIndicator, &checkboxstyle);

  //center
  checkboxstyle.rect = option.rect;
  checkboxstyle.rect.setLeft(option.rect.x() +
                             option.rect.width()/2 - checkbox_rect.width()/2);

  editor->setGeometry(checkboxstyle.rect);
}

QSize CheckBoxItemDelegate::sizeHint(const QStyleOptionViewItem& option,
                                     const QModelIndex& index) const
{
  // QStyleOptionButton checkboxstyle;
  // QRect checkbox_rect = QApplication::style()->subElementRect(QStyle::SE_CheckBoxIndicator,
  //                                                             &checkboxstyle);
  // return checkbox_rect.size();
  return QSize(10,10);
}

AUTOXTIME_UI_NAMESPACE_END
