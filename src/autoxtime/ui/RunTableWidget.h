#ifndef AUTOXTIME_UI_RUNTABLEWIDGET
#define AUTOXTIME_UI_RUNTABLEWIDGET

#include <autoxtime/ui/ui.h>
#include <QWidget>

class QTableView;

namespace autoxtime { namespace ui { class MultiSortFilterProxyModel; } }
namespace autoxtime { namespace ui { class RunTableModel; } }

AUTOXTIME_UI_NAMESPACE_BEG

class RunTableWidget : public QWidget
{
  Q_OBJECT

 public:
  explicit RunTableWidget(QWidget* pParent = nullptr);
  virtual ~RunTableWidget() = default;

  inline RunTableModel* model();
  inline MultiSortFilterProxyModel* proxyModel() const;
  inline QTableView* view() const;

 public slots:
  void setEventId(std::int64_t eventId);

 private:
  // table
  QTableView* mpTableView;
  RunTableModel* mpRunTableModel;
  MultiSortFilterProxyModel* mpSortFilterProxyModel;
};

inline RunTableModel* RunTableWidget::model()
{
  return mpRunTableModel;
}

inline MultiSortFilterProxyModel* RunTableWidget::proxyModel() const
{
  return mpSortFilterProxyModel;
}

inline QTableView* RunTableWidget::view() const
{
  return mpTableView;
}

AUTOXTIME_UI_NAMESPACE_END

#endif // AUTOXTIME_UI_RUNTABLEWIDGET
