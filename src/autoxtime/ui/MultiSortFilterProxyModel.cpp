#include <autoxtime/ui/MultiSortFilterProxyModel.h>
#include <autoxtime/log/Log.h>

AUTOXTIME_UI_NAMESPACE_BEG

MultiSortFilterProxyModel::MultiSortFilterProxyModel(QObject* pParent)
    : QSortFilterProxyModel(pParent),
      mSmartFilterRegexes()
{}

bool MultiSortFilterProxyModel
::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
  if (mFilterKeyColumnList.size() <= 1)
  {
    return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
  }

  QVector<bool> match_idxs(mSmartFilterRegexes.size(), false);

  // try to match against the columns in our list
  for (int column : mFilterKeyColumnList)
  {
    QModelIndex source_index = sourceModel()->index(sourceRow, column, sourceParent);
    QString key = sourceModel()->data(source_index, filterRole()).toString();
    // try to match against all of our smart filters for each column,
    // keeptring track of what filters matched
    for (int i = 0; i < mSmartFilterRegexes.size(); ++i)
    {
      // partial optimization (saves regex matching)
      // TODO skip the index entirely, if that makes sense?
      if (!match_idxs[i] && key.contains(mSmartFilterRegexes[i]))
      {
        match_idxs[i] = true;
      }
    }
  }
  // if all filters matched, we succeeeded (returned -1 from indexOf)
  // else at least one index didn't match, so failed
  return match_idxs.indexOf(false) == -1;
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

void MultiSortFilterProxyModel::setFilterSmart(const QString& filter)
{
  QList<QString> parts = filter.trimmed().split(' ');
  mSmartFilterRegexes.clear();
  for (const QString& p : parts)
  {
    mSmartFilterRegexes.push_back(QRegularExpression("^.*?" + p + ".*?$",
                                                     QRegularExpression::CaseInsensitiveOption));
  }
  // AXT_DEBUG << "set regex filter: " << mSmartFilterRegexes;

  // set wildcard filter in case we're only looking at one column
  setFilterWildcard(filter);
}

AUTOXTIME_UI_NAMESPACE_END
