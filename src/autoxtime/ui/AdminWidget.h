#ifndef AUTOXTIME_UI_ADMINWIDGET
#define AUTOXTIME_UI_ADMINWIDGET

#include <autoxtime/ui/ui.h>
#include <QWidget>
#include <QTreeWidgetItem>

#include <autoxtime/db/EventModel.h>
#include <autoxtime/db/OrganizationModel.h>

class QDateEdit;
class QFrame;
class QLabel;
class QLineEdit;
class QPushButton;

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
  void addClicked(bool checked = false);
  void deleteClicked(bool checked = false);
  void eventSaveClicked(bool checked = false);
  void eventCancelClicked(bool checked = false);

 private:
  enum State
  {
    STATE_DEFAULT,
    STATE_ADDING
  };
  enum TreeItemType
  {
    TREE_ITEM_TYPE_ORGANIZATION = QTreeWidgetItem::UserType + 1,
    TREE_ITEM_TYPE_EVENT        = QTreeWidgetItem::UserType + 2
  };
  enum TreeColumn
  {
    TREE_COLUMN_NAME = 0,
    TREE_COLUMN_DATE = 1,
    TREE_COLUMN_ID   = 2
  };
  enum TreeRole
  {
    TREE_ROLE_ID = Qt::UserRole + 1
  };

  void rebuildTree();
  void rebuildTreeOrganization(const autoxtime::db::OrganizationModel::ProtoPtr& pOrg);
  void rebuildTreeEvent(const autoxtime::db::EventModel::ProtoPtr& pEvent);

  AdminWidget::State mState;
  QTreeWidget* mpTree;
  QPushButton* mpAddButton;
  QPushButton* mpDeleteButton;
  QFrame* mpEventFrame;
  QLineEdit* mpEventNameLineEdit;
  QDateEdit* mpEventDateEdit;
  QPushButton* mpEventSaveButton;
  QPushButton* mpEventCancelButton;

  QTreeWidgetItem* mpAddItem;

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
