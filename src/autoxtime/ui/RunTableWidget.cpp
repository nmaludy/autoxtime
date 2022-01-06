#include <autoxtime/ui/RunTableWidget.h>

#include <autoxtime/log/Log.h>
#include <autoxtime/ui/CheckBoxItemDelegate.h>
#include <autoxtime/ui/MultiSortFilterProxyModel.h>
#include <autoxtime/ui/RunTableModel.h>

#include <QGridLayout>
#include <QHeaderView>
#include <QTableView>

AUTOXTIME_UI_NAMESPACE_BEG

RunTableWidget::RunTableWidget(QWidget* pParent)
    : QWidget(pParent),
      mpTableView(new QTableView(this)),
      mpRunTableModel(new RunTableModel(this)),
      mpSortFilterProxyModel(new MultiSortFilterProxyModel(this))
{
  QGridLayout* p_layout = new QGridLayout(this);

  // table
  {
    mpSortFilterProxyModel->setSourceModel(mpRunTableModel);
    mpTableView->setModel(mpSortFilterProxyModel);

    // enable column headers
    mpTableView->horizontalHeader()->setVisible(true);
    // enable row numbers
    mpTableView->verticalHeader()->setVisible(true);
    // enable sorting
    mpTableView->setSortingEnabled(false);
    // mpTableView->sortByColumn(RunTableModel::TABLE_COLUMN_DRIVER_NAME,
    //                           Qt::AscendingOrder);

    // setup check boxes
    mpTableView->setItemDelegateForColumn(RunTableModel::TABLE_COLUMN_DNF,
                                          new CheckBoxItemDelegate(this));

    p_layout->addWidget(mpTableView, 1, 0, 1, -1);
    p_layout->setRowStretch(1, 1);

    // load data from database
    mpRunTableModel->reset();
  }
}

void RunTableWidget::setEventId(std::int64_t eventId)
{
  mpRunTableModel->setEventId(eventId);
}

AUTOXTIME_UI_NAMESPACE_END
