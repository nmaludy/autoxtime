#ifndef AUTOXTIME_UI_REGISTRATIONTABLEWIDGET
#define AUTOXTIME_UI_REGISTRATIONTABLEWIDGET

#include <autoxtime/ui/ui.h>
#include <QList>
#include <QWidget>

class QLineEdit;
class QTableView;

namespace autoxtime { namespace ui { class MultiSortFilterProxyModel; } }
namespace autoxtime { namespace ui { class RegistrationTableModel; } }
namespace autoxtime { namespace ui { class RegistrationTableModel; } }

AUTOXTIME_UI_NAMESPACE_BEG

class RegistrationTableWidget : public QWidget
{
  Q_OBJECT

 public:
  explicit RegistrationTableWidget(QWidget* pParent = nullptr);

  inline RegistrationTableModel* model() const;
  inline MultiSortFilterProxyModel* proxyModel() const;
  inline QTableView* view() const;

 signals:
  void numEntriesChanged(std::uint64_t numEntries,
                         std::uint64_t numCheckInEntries);

 public slots:
  void setEventId(std::int64_t eventId);
  void modelNumEntriesChanged(std::uint64_t numEntries,
                              std::uint64_t numCheckInEntries);
  void filterChanged(const QString& text);
  void filterColumnToggled();

 private:

  // filter
  QLineEdit* mpFilterLineEdit;

  // table
  QTableView* mpTableView;
  RegistrationTableModel* mpRegistrationTableModel;
  MultiSortFilterProxyModel* mpSortFilterProxyModel;

  QStringList mTableColumnHeaders;
  QList<int> mTableFilterColumns;
  QList<QAction*> mTableFilterColumnActions;
};

inline RegistrationTableModel* RegistrationTableWidget::model() const
{
  return mpRegistrationTableModel;
}

inline MultiSortFilterProxyModel* RegistrationTableWidget::proxyModel() const
{
  return mpSortFilterProxyModel;
}

inline QTableView* RegistrationTableWidget::view() const
{
  return mpTableView;
}

AUTOXTIME_UI_NAMESPACE_END

#endif // AUTOXTIME_UI_REGISTRATIONTABLEWIDGET
