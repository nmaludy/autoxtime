#include <autoxtime/ui/MultiSortFilterProxyModel.h>
#include <autoxtime/log/Log.h>

AUTOXTIME_UI_NAMESPACE_BEG

MultiSortFilterProxyModel::MultiSortFilterProxyModel(QObject* pParent)
    : QSortFilterProxyModel(pParent),
      mFilterKeyColumnList(),
      mSmartFilterRegexes(),
      mSortLinkedListCurrentColumn(-1),
      mSortLinkedListPreviousColumn(-1)
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
    mSmartFilterRegexes << QRegularExpression("^.*?" + p + ".*?$",
                                              QRegularExpression::CaseInsensitiveOption);
  }
  // AXT_DEBUG << "set regex filter: " << mSmartFilterRegexes;

  // set wildcard filter in case we're only looking at one column
  setFilterWildcard(filter);
}

QVariant MultiSortFilterProxyModel::headerData(int section,
                                               Qt::Orientation orientation,
                                               int role) const
{
  // return row numbers for vertical header, but only if source model
  // returns a null/invalid QVariant
  QVariant data = sourceModel()->headerData(section, orientation, role);
  if (data.isNull() && role == Qt::DisplayRole && orientation == Qt::Vertical)
  {
    data = QVariant(section + 1);
  }
  return data;
}

void MultiSortFilterProxyModel::setSortLinkedListColumns(int currentCol, int previousCol)
{
  mSortLinkedListCurrentColumn = currentCol;
  mSortLinkedListPreviousColumn = previousCol;
  invalidate();
}

bool MultiSortFilterProxyModel::lessThan(const QModelIndex& sourceLeft,
                                         const QModelIndex& sourceRight) const
{
  // enable sorting of linked-list columns
  if (mSortLinkedListCurrentColumn != -1 && mSortLinkedListPreviousColumn != -1 &&
      sourceLeft.column() == mSortLinkedListCurrentColumn &&
      sourceRight.column() == mSortLinkedListCurrentColumn)
  {
    QModelIndex left_pre_idx = sourceModel()->index(sourceLeft.row(), mSortLinkedListPreviousColumn);
    QModelIndex right_pre_idx = sourceModel()->index(sourceRight.row(), mSortLinkedListPreviousColumn);
    QVariant left_pre_data = sourceModel()->data(left_pre_idx);
    QVariant left_cur_data = sourceModel()->data(sourceLeft);
    QVariant right_pre_data = sourceModel()->data(right_pre_idx);
    QVariant right_cur_data = sourceModel()->data(sourceRight);

    // the start of the linked list has no previous element
    if (left_pre_data.isNull())
    {
      AXT_DEBUG << "left is head = " << left_pre_data.toString();
      return true;
    }
    if (right_pre_data.isNull())
    {
      AXT_DEBUG << "right is head = " << right_pre_data.toString();
      return false;
    }

    // if the left ID is the same as the right's previous ID
    // then left is less than right
    AXT_DEBUG << "left pre = " << left_pre_data.toString()
              << "  left cur = " << left_cur_data.toString()
              << "  right pre = " << right_pre_data.toString()
              << "  right cur = " << right_cur_data.toString()
              << "  left==right = " << (left_cur_data == right_pre_data);
    return left_cur_data != right_pre_data;
  }
  return QSortFilterProxyModel::lessThan(sourceLeft, sourceRight);
}

AUTOXTIME_UI_NAMESPACE_END
