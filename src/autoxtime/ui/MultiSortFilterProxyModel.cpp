#include <autoxtime/ui/MultiSortFilterProxyModel.h>

AUTOXTIME_UI_NAMESPACE_BEG

MultiSortFilterProxyModel::MultiSortFilterProxyModel(QObject* pParent)
    : QSortFilterProxyModel(pParent)
{}

bool MultiSortFilterProxyModel
::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
  if (mFilterKeyColumnList.size() <= 1)
  {
    return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
  }

  // try to match against the columns in our list
  for (int column : mFilterKeyColumnList)
  {
    QModelIndex source_index = sourceModel()->index(sourceRow, column, sourceParent);
    QString key = sourceModel()->data(source_index, filterRole()).toString();
    if (key.contains(filterRegExp()))
    {
      return true;
    }
  }
  return false;
}

void MultiSortFilterProxyModel::setFilterKeyColumnList(const QList<int>& columns)
{
  mFilterKeyColumnList = columns;
  if (mFilterKeyColumnList.size() == 1)
  {
    setFilterKeyColumn(mFilterKeyColumnList.front());
  }
  else
  {
    invalidateFilter();
  }
}

AUTOXTIME_UI_NAMESPACE_END
