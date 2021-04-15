#ifndef AUTOXTIME_UI_FILTERHIGHLIGHTITEMDELEGATE
#define AUTOXTIME_UI_FILTERHIGHLIGHTITEMDELEGATE

#include <autoxtime/ui/ui.h>

#include <QStyledItemDelegate>

AUTOXTIME_UI_NAMESPACE_BEG

class FilterHighlightItemDelegate : public QStyledItemDelegate
{
  Q_OBJECT

 public:
  explicit FilterHighlightItemDelegate(QObject* parent = nullptr);
  virtual ~FilterHighlightItemDelegate() = default;

  virtual void paint(QPainter* painter,
                     const QStyleOptionViewItem& option,
                     const QModelIndex& index) const override;
};

AUTOXTIME_UI_NAMESPACE_END

#endif // AUTOXTIME_UI_FILTERHIGHLIGHTITEMDELEGATE
