#ifndef AUTOXTIME_UI_ADMINWIDGET
#define AUTOXTIME_UI_ADMINWIDGET

#include <autoxtime/ui/ui.h>
#include <QWidget>

#include <autoxtime/db/EventModel.h>
#include <autoxtime/db/OrganizationModel.h>

class QDateEdit;
class QFrame;
class QLabel;
class QLineEdit;
class QPushButton;
class QTreeWidget;
class QTreeWidgetItem;

AUTOXTIME_UI_NAMESPACE_BEG

class AdminWidget : public QWidget
{
  Q_OBJECT

 public:
  explicit AdminWidget(QWidget* pParent = nullptr);

 public slots:
  void setOrganizations(const autoxtime::db::OrganizationModel::ProtoPtrVec& orgs);
  void setEvents(const autoxtime::db::EventModel::ProtoPtrVec& events);
  void treeSelectionChanged(QTreeWidgetItem* pCurrent, QTreeWidgetItem* pPrevious);

 private:
  static const int TREE_ITEM_TYPE_ORGANIZATION;
  static const int TREE_ITEM_TYPE_EVENT;

  static const int TREE_COL_NAME;
  static const int TREE_COL_DATE;

  void rebuildTree();

  // QLabel* mpLabel;
  QTreeWidget* mpTree;
  QPushButton* mpAddButton;
  QPushButton* mpDeleteButton;
  QFrame* mpEventFrame;
  QLineEdit* mpEventNameLineEdit;
  QDateEdit* mpEventDateEdit;

  autoxtime::db::OrganizationModel* mpOrganizationModel;
  autoxtime::db::OrganizationModel::ProtoPtrVec mOrganizations;
  // map from organization id -> tree item
  std::unordered_map<std::int32_t, QTreeWidgetItem*> mOrganizationTreeItems;

  autoxtime::db::EventModel* mpEventModel;
  autoxtime::db::EventModel::ProtoPtrVec mEvents;
  // map from event id -> tree item
  // we could store this in some sort of nested map, but we don't need that level
  // of complexity yet, just care if we know about this event or not
  std::unordered_map<std::int32_t, QTreeWidgetItem*> mEventTreeItems;
};

AUTOXTIME_UI_NAMESPACE_END

#endif // AUTOXTIME_UI_ADMINWIDGET
