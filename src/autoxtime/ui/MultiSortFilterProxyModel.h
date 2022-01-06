#ifndef AUTOXTIME_UI_MULTISORTFILTERPROXYMODEL
#define AUTOXTIME_UI_MULTISORTFILTERPROXYMODEL

#include <autoxtime/ui/ui.h>

#include <QList>
#include <QSortFilterProxyModel>
#include <QRegularExpression>

AUTOXTIME_UI_NAMESPACE_BEG

class MultiSortFilterProxyModel : public QSortFilterProxyModel
{
  Q_OBJECT

 public:
  explicit MultiSortFilterProxyModel(QObject* pParent = nullptr);
  virtual ~MultiSortFilterProxyModel() = default;

  inline const QList<int>& filterKeyColumnList() const;
  void setFilterKeyColumnList(const QList<int>& columns);
  void setFilterSmart(const QString& filter);
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  inline int sortLinkedListCurrentColumn() const;
  inline int sortLinkedListPreviousColumn() const;
  void setSortLinkedListColumns(int currentCol, int previousCol);

 protected:
  virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
  virtual bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

 private:
  QList<int> mFilterKeyColumnList;
  QList<QRegularExpression> mSmartFilterRegexes;
  int mSortLinkedListCurrentColumn;
  int mSortLinkedListPreviousColumn;
};

inline const QList<int>& MultiSortFilterProxyModel::filterKeyColumnList() const
{
  return mFilterKeyColumnList;
}

inline int MultiSortFilterProxyModel::sortLinkedListCurrentColumn() const
{
  return mSortLinkedListCurrentColumn;
}

inline int MultiSortFilterProxyModel::sortLinkedListPreviousColumn() const
{
  return mSortLinkedListPreviousColumn;
}

AUTOXTIME_UI_NAMESPACE_END

#endif // AUTOXTIME_UI_MULTISORTFILTERPROXYMODEL
