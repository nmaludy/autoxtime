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

 protected:
  virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

 private:
  QList<int> mFilterKeyColumnList;
  QList<QRegularExpression> mSmartFilterRegexes;
};

inline const QList<int>& MultiSortFilterProxyModel::filterKeyColumnList() const
{
  return mFilterKeyColumnList;
}

AUTOXTIME_UI_NAMESPACE_END

#endif // AUTOXTIME_UI_MULTISORTFILTERPROXYMODEL
