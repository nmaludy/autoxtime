#include <autoxtime/ui/RegistrationTableWidget.h>

#include <autoxtime/log/Log.h>
#include <autoxtime/ui/CheckBoxItemDelegate.h>
#include <autoxtime/ui/MultiSortFilterProxyModel.h>
#include <autoxtime/ui/RegistrationTableModel.h>

#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QSplitter>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QTableView>
#include <QToolButton>

AUTOXTIME_UI_NAMESPACE_BEG

RegistrationTableWidget::RegistrationTableWidget(QWidget* pParent)
    : QWidget(pParent),
      mpFilterLineEdit(new QLineEdit(this)),
      mpTableView(new QTableView(this)),
      mpRegistrationTableModel(new RegistrationTableModel(this)),
      mpSortFilterProxyModel(new MultiSortFilterProxyModel(this))
{
  QGridLayout* p_layout = new QGridLayout(this);
  // TODO make this configurable
  mTableColumnHeaders << "First Name"
                      << "Last Name"
                      << "Class"
                      << "Number"
      // << "Color"
                      << "Car Model"
                      << "✓ In";
  // TODO make this configurable
  mTableFilterColumns << RegistrationTableModel::TABLE_COLUMN_FIRST_NAME
                      << RegistrationTableModel::TABLE_COLUMN_LAST_NAME
                      << RegistrationTableModel::TABLE_COLUMN_CLASS
                      << RegistrationTableModel::TABLE_COLUMN_CAR_NUM
                      << RegistrationTableModel::TABLE_COLUMN_CAR_MODEL;

  // filter
  {
    connect(mpFilterLineEdit, &QLineEdit::textChanged,
            this,             &RegistrationTableWidget::filterChanged);

    QMenu* p_menu = new QMenu(this);
    for (int i = 0; i < mTableColumnHeaders.size(); ++i)
    {
      const QString& hdr = mTableColumnHeaders.at(i);
      QAction* p_action = p_menu->addAction(hdr);
      p_action->setData(i);
      p_action->setCheckable(true);
      p_action->setChecked(mTableFilterColumns.contains(i));
      mTableFilterColumnActions << p_action;
      connect(p_action, &QAction::toggled,
              this,     &RegistrationTableWidget::filterColumnToggled);
    }

    QPushButton* p_filter_button = new QPushButton("Filter", this);
    p_filter_button->setMenu(p_menu);
    p_layout->addWidget(p_filter_button, 0, 0);
    p_layout->addWidget(mpFilterLineEdit, 0, 1, 1, -1);
  }

  // table
  {
    mpSortFilterProxyModel->setFilterKeyColumnList(mTableFilterColumns);
    mpSortFilterProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    mpSortFilterProxyModel->setSourceModel(mpRegistrationTableModel);
    mpTableView->setModel(mpSortFilterProxyModel);

    // enable column headers
    mpTableView->horizontalHeader()->setVisible(true);
    // disable row numbers
    mpTableView->verticalHeader()->setVisible(false);
    // enable sorting
    mpTableView->setSortingEnabled(true);
    mpTableView->sortByColumn(RegistrationTableModel::TABLE_COLUMN_FIRST_NAME,
                              Qt::AscendingOrder);
    // hide color column
    mpTableView->setColumnHidden(RegistrationTableModel::TABLE_COLUMN_CAR_COLOR, true);

    // setup check boxes
    mpTableView->setItemDelegateForColumn(RegistrationTableModel::TABLE_COLUMN_CHECKED_IN,
                                          new CheckBoxItemDelegate(this));

    // signals/slots
    connect(mpRegistrationTableModel, &RegistrationTableModel::numEntriesChanged,
            this,                     &RegistrationTableWidget::modelNumEntriesChanged);

    p_layout->addWidget(mpTableView, 1, 0, 1, -1);
    p_layout->setRowStretch(1, 1);

    // load data from database
    mpRegistrationTableModel->reset();
  }
}

void RegistrationTableWidget::setEventId(std::int64_t eventId)
{
  mpRegistrationTableModel->setEventId(eventId);
}

void RegistrationTableWidget::modelNumEntriesChanged(std::uint64_t numEntries,
                                                     std::uint64_t numCheckInEntries)
{
  emit numEntriesChanged(numEntries, numCheckInEntries);
}

void RegistrationTableWidget::filterChanged(const QString& text)
{
  mpSortFilterProxyModel->setFilterSmart(text);
}

void RegistrationTableWidget::filterColumnToggled()
{
  mTableFilterColumns.clear();
  for (int i = 0; i < mTableFilterColumnActions.size(); ++i)
  {
    QAction* p_action = mTableFilterColumnActions.at(i);
    if (p_action->isChecked())
    {
      mTableFilterColumns << i;
    }
  }
  mpSortFilterProxyModel->setFilterKeyColumnList(mTableFilterColumns);
}

AUTOXTIME_UI_NAMESPACE_END
