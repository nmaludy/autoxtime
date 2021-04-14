#ifndef AUTOXTIME_UI_CHECKBOXITEMDELEGATE
#define AUTOXTIME_UI_CHECKBOXITEMDELEGATE

#include <autoxtime/ui/ui.h>

#include <QObject>
#include <QStyledItemDelegate>

AUTOXTIME_UI_NAMESPACE_BEG

class CheckBoxItemDelegate : public QStyledItemDelegate
{
  Q_OBJECT

 public:
  explicit CheckBoxItemDelegate(QObject* parent = nullptr);


  virtual QWidget* createEditor(QWidget* parent,
                                const QStyleOptionViewItem& option,
                                const QModelIndex& index) const override;

  virtual void setEditorData(QWidget* editor, const QModelIndex& index) const override;

  virtual void setModelData(QWidget* editor,
                            QAbstractItemModel* model,
                            const QModelIndex& index) const  override;

  virtual void paint(QPainter* painter,
                     const QStyleOptionViewItem& option,
                     const QModelIndex& index) const override;

  virtual void updateEditorGeometry(QWidget* editor,
                                    const QStyleOptionViewItem& option,
                                    const QModelIndex& index) const override;

  virtual bool editorEvent(QEvent* event,
                           QAbstractItemModel* model,
                           const QStyleOptionViewItem& option,
                           const QModelIndex& index) override;
 public slots:
  void commitAndCloseEditor();

 private:
  Qt::CheckState indexCheckState(const QModelIndex& index) const
  {
    return Qt::CheckState(qvariant_cast<int>(index.data(Qt::CheckStateRole)));
  }
};

AUTOXTIME_UI_NAMESPACE_END

#endif // AUTOXTIME_UI_CHECKBOXITEMDELEGATE
