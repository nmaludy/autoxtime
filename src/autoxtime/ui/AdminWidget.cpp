#include <autoxtime/ui/AdminWidget.h>

#include <autoxtime/proto/organization.pb.h>
#include <autoxtime/proto/event.pb.h>

#include <QDateEdit>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTreeWidget>

AUTOXTIME_UI_NAMESPACE_BEG

const int AdminWidget::TREE_ITEM_TYPE_ORGANIZATION = QTreeWidgetItem::UserType + 1;
const int AdminWidget::TREE_ITEM_TYPE_EVENT        = QTreeWidgetItem::UserType + 2;

const int AdminWidget::TREE_COL_NAME = 0;
const int AdminWidget::TREE_COL_DATE = 1;

AdminWidget::AdminWidget(QWidget* pParent)
    : QWidget(pParent),
      mpTree(new QTreeWidget(this)),
      mpAddButton(new QPushButton("add", this)),
      mpDeleteButton(new QPushButton("delete", this)),
      mpEventFrame(new QFrame(this)),
      mpEventNameLineEdit(new QLineEdit(this)),
      mpEventDateEdit(new QDateEdit(this)),
      mpOrganizationModel(new autoxtime::db::OrganizationModel(this)),
      mOrganizations(),
      mpEventModel(new autoxtime::db::EventModel(this)),
      mEvents()
{
  // root layout
  QGridLayout* p_layout = new QGridLayout(this);

  // tree
  {
    QFrame* p_tree_frame = new QFrame(this);
    QGridLayout* p_tree_layout = new QGridLayout(p_tree_frame);

    QStringList headers;
    headers << "Events";
    headers << "Date";
    mpTree->setHeaderLabels(headers);
    mpTree->setSortingEnabled(true);
    mpTree->sortByColumn(TREE_COL_NAME, Qt::AscendingOrder);
    p_tree_layout->addWidget(mpTree, 0, 0, 1, -1);

    // buttons
    // TODO, connect them to something
    p_tree_layout->addWidget(mpAddButton, 1, 0);
    p_tree_layout->addWidget(mpDeleteButton, 1, 1);

    // add tree to main layout
    p_layout->addWidget(p_tree_frame, 0, 0);
  }

  // Event frame
  {
    mpEventDateEdit->setCalendarPopup(true);

    QGridLayout* p_evemt_layout = new QGridLayout(mpEventFrame);
    p_evemt_layout->addWidget(new QLabel("Name", this), 0, 0);
    p_evemt_layout->addWidget(mpEventNameLineEdit, 0, 1, 1, -1);
    p_evemt_layout->addWidget(new QLabel("Date", this), 1, 0);
    p_evemt_layout->addWidget(mpEventDateEdit, 1, 1, 1, -1);
    p_evemt_layout->setRowStretch(2, 1);

    // add event editor to main layout
    p_layout->addWidget(mpEventFrame, 0, 1);

    // disable until we click on an event
    mpEventFrame->setEnabled(false);
    mpEventNameLineEdit->setEnabled(false);
    mpEventDateEdit->setEnabled(false);
  }

  // connect our signals/ slots
  connect(mpOrganizationModel, &autoxtime::db::OrganizationModel::listResult,
          this,                &AdminWidget::setOrganizations);
  connect(mpEventModel, &autoxtime::db::EventModel::listResult,
          this,         &AdminWidget::setEvents);
  connect(mpTree, &QTreeWidget::currentItemChanged,
          this,   &AdminWidget::treeSelectionChanged);

  // kick off async queries
  mpEventModel->listAsync();
  mpOrganizationModel->listAsync();
}

void AdminWidget::setOrganizations(const autoxtime::db::OrganizationModel::ProtoPtrVec& orgs)
{
  mOrganizations = orgs;
  rebuildTree();
}

void AdminWidget::setEvents(const autoxtime::db::EventModel::ProtoPtrVec& events)
{
  mEvents = events;
  rebuildTree();
}

void AdminWidget::rebuildTree()
{
  for (const autoxtime::db::OrganizationModel::ProtoPtr& p_org : mOrganizations)
  {
    // do we know about this organization yet or not?
    std::int32_t org_id = p_org->organization_id();
    std::unordered_map<std::int32_t, QTreeWidgetItem*>::iterator iter =
        mOrganizationTreeItems.find(org_id);

    // if we already have it, use existing
    // else add a new one
    QTreeWidgetItem* p_org_item = nullptr;
    if (iter != mOrganizationTreeItems.end())
    {
      p_org_item = iter->second;
    }
    else
    {
      p_org_item = new QTreeWidgetItem(mpTree, AdminWidget::TREE_ITEM_TYPE_ORGANIZATION);
      // expand the organization item (to show all events) when first added
      // if the user hides it, we dont' want to mess with the expanded value after this
      p_org_item->setExpanded(true);
      mOrganizationTreeItems[org_id] = p_org_item;
    }

    // set new items and update existing items with the current properties,
    // just in case it changes
    p_org_item->setText(TREE_COL_NAME, QString::fromStdString(p_org->name()));
  }

  for (const autoxtime::db::EventModel::ProtoPtr& p_event : mEvents)
  {
    // do we know about this event yet or not?
    std::int32_t event_id = p_event->event_id();
    std::unordered_map<std::int32_t, QTreeWidgetItem*>::iterator event_iter =
        mEventTreeItems.find(event_id);

    QTreeWidgetItem* p_event_item = nullptr;
    if (event_iter != mEventTreeItems.end())
    {
      // use existing
      p_event_item = event_iter->second;
    }
    else
    {
      // create new, first we need to find the org for this event
      std::int32_t org_id = p_event->organization_id();
      std::unordered_map<std::int32_t, QTreeWidgetItem*>::iterator org_iter =
          mOrganizationTreeItems.find(org_id);

      // if we already have it, use existing
      // else add a new one
      QTreeWidgetItem* p_org_item = nullptr;
      if (org_iter != mOrganizationTreeItems.end())
      {
        p_org_item = org_iter->second;
      }
      else
      {
        // Received an event from an unknown org, this is OK because our orgs should
        // be coming (we kicked them off in parallel, so they might be slightly offset
        // with regard to order of arrival)
        continue;
      }

      // creates a new item and adds it as a child under the organization
      p_event_item = new QTreeWidgetItem(p_org_item, AdminWidget::TREE_ITEM_TYPE_EVENT);
      mEventTreeItems[event_id] = p_event_item;
    }

    // set new items and update existing items with the current properties,
    // just in case it changes
    p_event_item->setText(TREE_COL_NAME, QString::fromStdString(p_event->name()));
    p_event_item->setText(TREE_COL_DATE, QString::fromStdString(p_event->date()));
  }
}

void AdminWidget::treeSelectionChanged(QTreeWidgetItem* pCurrent, QTreeWidgetItem* pPrevious)
{
  bool b_event_item = pCurrent && pCurrent->type() == TREE_ITEM_TYPE_EVENT;
  if (b_event_item)
  {
    mpEventNameLineEdit->setText(pCurrent->text(TREE_COL_NAME));
    QDate d = QDate::fromString(pCurrent->text(TREE_COL_DATE), Qt::ISODate);
    mpEventDateEdit->setDate(d);
  }

  mpEventFrame->setEnabled(b_event_item);
  mpEventNameLineEdit->setEnabled(b_event_item);
  mpEventDateEdit->setEnabled(b_event_item);
}

AUTOXTIME_UI_NAMESPACE_END
