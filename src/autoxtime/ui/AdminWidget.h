#ifndef AUTOXTIME_UI_ADMINWIDGET
#define AUTOXTIME_UI_ADMINWIDGET

#include <autoxtime/ui/ui.h>
#include <QWidget>
#include <QTreeWidgetItem>

class QDateEdit;
class QFrame;
class QLabel;
class QLineEdit;
class QPushButton;

namespace autoxtime { namespace proto { class Organization; } }
namespace autoxtime { namespace proto { class Season; } }
namespace autoxtime { namespace proto { class Event; } }
namespace autoxtime { namespace db { class OrganizationModel; } }
namespace autoxtime { namespace db { class SeasonModel; } }
namespace autoxtime { namespace db { class EventModel; } }

AUTOXTIME_UI_NAMESPACE_BEG

class EventWidget;

class AdminWidget : public QWidget
{
  Q_OBJECT

 public:
  explicit AdminWidget(QWidget* pParent = nullptr);

 public slots:
  void setOrganizations(const std::vector<std::shared_ptr<autoxtime::proto::Organization>>& orgs);
  void setSeasons(const std::vector<std::shared_ptr<autoxtime::proto::Season>>& seasons);
  void setEvents(const std::vector<std::shared_ptr<autoxtime::proto::Event>>& events);
  void treeSelectionChanged(QTreeWidgetItem* pCurrent, QTreeWidgetItem* pPrevious);
  void addClicked(bool checked = false);
  void deleteClicked(bool checked = false);

  void eventSaved(const autoxtime::proto::Event& event);
  void eventCancelled();


 private:
  enum State
  {
    STATE_DEFAULT,
    STATE_ADDING
  };
  enum TreeItemType
  {
    TREE_ITEM_TYPE_ORGANIZATION = QTreeWidgetItem::UserType + 1,
    TREE_ITEM_TYPE_SEASON       = QTreeWidgetItem::UserType + 2,
    TREE_ITEM_TYPE_EVENT        = QTreeWidgetItem::UserType + 3
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
  void rebuildTreeOrganization(const std::shared_ptr<autoxtime::proto::Organization>& pOrg);
  void rebuildTreeSeason(const std::shared_ptr<autoxtime::proto::Season>& pSeason);
  void rebuildTreeEvent(const std::shared_ptr<autoxtime::proto::Event>& pEvent);

  AdminWidget::State mState;
  QTreeWidget* mpTree;
  QPushButton* mpAddButton;
  QPushButton* mpDeleteButton;
  EventWidget* mpEventWidget;

  QTreeWidgetItem* mpAddItem;

  autoxtime::db::OrganizationModel* mpOrganizationModel;
  std::vector<std::shared_ptr<autoxtime::proto::Organization>> mOrganizations;
  // map from organization id -> tree item
  std::unordered_map<std::int64_t, QTreeWidgetItem*> mOrganizationTreeItems;

  autoxtime::db::SeasonModel* mpSeasonModel;
  std::vector<std::shared_ptr<autoxtime::proto::Season>> mSeasons;
  // map from  id -> tree item
  std::unordered_map<std::int64_t, QTreeWidgetItem*> mSeasonTreeItems;

  autoxtime::db::EventModel* mpEventModel;
  std::vector<std::shared_ptr<autoxtime::proto::Event>> mEvents;
  // map from event id -> tree item
  // we could store this in some sort of nested map, but we don't need that level
  // of complexity yet, just care if we know about this event or not
  std::unordered_map<std::int64_t, QTreeWidgetItem*> mEventTreeItems;
};

AUTOXTIME_UI_NAMESPACE_END

#endif // AUTOXTIME_UI_ADMINWIDGET
