#include <autoxtime/ui/AdminWidget.h>

// autoxtime
#include <autoxtime/log/Log.h>
#include <autoxtime/db/EventModel.h>
#include <autoxtime/db/OrganizationModel.h>
#include <autoxtime/db/SeasonModel.h>
#include <autoxtime/proto/event.pb.h>
#include <autoxtime/proto/organization.pb.h>
#include <autoxtime/proto/season.pb.h>
#include <autoxtime/ui/EventWidget.h>

// Qt
#include <QDateEdit>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTreeWidget>

AUTOXTIME_UI_NAMESPACE_BEG

AdminWidget::AdminWidget(QWidget* pParent)
    : QWidget(pParent),
      mState(AdminWidget::STATE_DEFAULT),
      mpTree(new QTreeWidget(this)),
      mpAddButton(new QPushButton("Add", this)),
      mpDeleteButton(new QPushButton("Delete", this)),
      mpEventWidget(new EventWidget(this)),
      mpAddItem(nullptr),
      mpOrganizationModel(new autoxtime::db::OrganizationModel(this)),
      mOrganizations(),
      mpSeasonModel(new autoxtime::db::SeasonModel(this)),
      mSeasons(),
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
    mpTree->sortByColumn(TREE_COLUMN_NAME, Qt::AscendingOrder);
    p_tree_layout->addWidget(mpTree, 0, 0, 1, -1);

    // buttons

    // disable until a valid selection is made
    mpAddButton->setEnabled(false);
    mpDeleteButton->setEnabled(false);
    connect(mpAddButton, &QPushButton::clicked,
            this,        &AdminWidget::addClicked);
    connect(mpDeleteButton, &QPushButton::clicked,
            this,           &AdminWidget::deleteClicked);

    p_tree_layout->addWidget(mpAddButton, 1, 0);
    p_tree_layout->addWidget(mpDeleteButton, 1, 1);

    // add tree to main layout
    p_layout->addWidget(p_tree_frame, 0, 0);
  }

  // Event frame
  {
    // add event editor to main layout
    p_layout->addWidget(mpEventWidget, 0, 1);

    // capture save/cancel events
    connect(mpEventWidget, &EventWidget::eventSaved,
            this,          &AdminWidget::eventSaved);
    connect(mpEventWidget, &EventWidget::eventCancelled,
            this,          &AdminWidget::eventCancelled);
  }

  // connect our signals/ slots
  connect(mpOrganizationModel, &autoxtime::db::OrganizationModel::listResult,
          this,                &AdminWidget::setOrganizations);
  connect(mpSeasonModel, &autoxtime::db::SeasonModel::listResult,
          this,          &AdminWidget::setSeasons);
  connect(mpEventModel, &autoxtime::db::EventModel::listResult,
          this,         &AdminWidget::setEvents);
  connect(mpTree, &QTreeWidget::currentItemChanged,
          this,   &AdminWidget::treeSelectionChanged);

  // kick off async queries
  mpOrganizationModel->listAsync();
  mpSeasonModel->listAsync();
  mpEventModel->listAsync();
}

void AdminWidget::setOrganizations(const std::vector<std::shared_ptr<autoxtime::proto::Organization>>& orgs)
{
  // TODO store these in a better way
  mOrganizations = orgs;
  rebuildTree();
}

void AdminWidget::setSeasons(const std::vector<std::shared_ptr<autoxtime::proto::Season>>& seasons)
{
  // TODO store these in a better way
  mSeasons = seasons;
  rebuildTree();
}

void AdminWidget::setEvents(const std::vector<std::shared_ptr<autoxtime::proto::Event>>& events)
{
  // TODO store these in a better way
  mEvents = events;
  rebuildTree();
}

void AdminWidget::rebuildTree()
{
  for (const std::shared_ptr<autoxtime::proto::Organization>& p_org : mOrganizations)
  {
    rebuildTreeOrganization(p_org);
  }
  for (const std::shared_ptr<autoxtime::proto::Season>& p_season : mSeasons)
  {
    rebuildTreeSeason(p_season);
  }
  for (const std::shared_ptr<autoxtime::proto::Event>& p_event : mEvents)
  {
    rebuildTreeEvent(p_event);
  }
}

void AdminWidget::rebuildTreeOrganization(const std::shared_ptr<autoxtime::proto::Organization>& pOrg)
{
  // do we know about this organization yet or not?
  std::int64_t org_id = pOrg->organization_id();
  std::unordered_map<std::int64_t, QTreeWidgetItem*>::iterator iter =
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
  p_org_item->setText(TREE_COLUMN_NAME, QString::fromStdString(pOrg->name()));
  p_org_item->setData(TREE_COLUMN_ID, TREE_ROLE_ID, QVariant::fromValue(pOrg->organization_id()));
}

void AdminWidget::rebuildTreeSeason(const std::shared_ptr<autoxtime::proto::Season>& pSeason)
{
  // do we know about this season yet or not?
  std::int64_t season_id = pSeason->season_id();
  std::unordered_map<std::int64_t, QTreeWidgetItem*>::iterator season_iter =
      mSeasonTreeItems.find(season_id);

  QTreeWidgetItem* p_season_item = nullptr;
  if (season_iter != mSeasonTreeItems.end())
  {
    // use existing
    p_season_item = season_iter->second;
  }
  else
  {
    // create new, first we need to find the org for this season
    std::int64_t org_id = pSeason->organization_id();
    std::unordered_map<std::int64_t, QTreeWidgetItem*>::iterator org_iter =
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
      // Received an season from an unknown org, this is OK because our orgs should
      // be coming (we kicked them off in parallel, so they might be slightly offset
      // with regard to order of arrival)
      return;
    }

    // creates a new item and adds it as a child under the organization
    p_season_item = new QTreeWidgetItem(p_org_item, AdminWidget::TREE_ITEM_TYPE_SEASON);
    mSeasonTreeItems[season_id] = p_season_item;
  }

  // set new items and update existing items with the current properties,
  // just in case it changes
  p_season_item->setText(TREE_COLUMN_NAME, QString::fromStdString(pSeason->name()));
  p_season_item->setData(TREE_COLUMN_ID, TREE_ROLE_ID, QVariant::fromValue(pSeason->season_id()));
}

void AdminWidget::rebuildTreeEvent(const std::shared_ptr<autoxtime::proto::Event>& pEvent)
{
  // do we know about this event yet or not?
  std::int64_t event_id = pEvent->event_id();
  std::unordered_map<std::int64_t, QTreeWidgetItem*>::iterator event_iter =
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
    std::int64_t season_id = pEvent->season_id();
    std::unordered_map<std::int64_t, QTreeWidgetItem*>::iterator season_iter =
        mSeasonTreeItems.find(season_id);

    // if we already have it, use existing
    // else add a new one
    QTreeWidgetItem* p_season_item = nullptr;
    if (season_iter != mSeasonTreeItems.end())
    {
      p_season_item = season_iter->second;
    }
    else
    {
      // Received an event from an unknown season, this is OK because our season should
      // be coming (we kicked them off in parallel, so they might be slightly offset
      // with regard to order of arrival)
      return;
    }

    // creates a new item and adds it as a child under the season
    p_event_item = new QTreeWidgetItem(p_season_item, AdminWidget::TREE_ITEM_TYPE_EVENT);
    mEventTreeItems[event_id] = p_event_item;
  }

  // set new items and update existing items with the current properties,
  // just in case it changes
  p_event_item->setText(TREE_COLUMN_NAME, QString::fromStdString(pEvent->name()));
  p_event_item->setText(TREE_COLUMN_DATE, QString::fromStdString(pEvent->date()));
  p_event_item->setData(TREE_COLUMN_ID, TREE_ROLE_ID, QVariant::fromValue(pEvent->event_id()));
}

void AdminWidget::treeSelectionChanged(QTreeWidgetItem* pCurrent, QTreeWidgetItem* pPrevious)
{
  AXT_INFO << "Selected: " << pCurrent;
  bool b_event_item = pCurrent && pCurrent->type() == TREE_ITEM_TYPE_EVENT;
  if (b_event_item)
  {
    std::shared_ptr<autoxtime::proto::Event> p_event =
        std::make_shared<autoxtime::proto::Event>();
    p_event->set_event_id(pCurrent->data(TREE_COLUMN_ID, TREE_ROLE_ID).toLongLong());

    // set the season from the parent item
    QTreeWidgetItem* p_season_item = pCurrent->parent();
    p_event->set_season_id(p_season_item->data(TREE_COLUMN_ID, TREE_ROLE_ID).toLongLong());

    // set the organization from the grandparent item
    QTreeWidgetItem* p_org_item = p_season_item->parent();
    p_event->set_organization_id(p_org_item->data(TREE_COLUMN_ID, TREE_ROLE_ID).toLongLong());

    // don't put the fake name in the text box
    if (pCurrent != mpAddItem)
    {
      p_event->set_name(pCurrent->text(TREE_COLUMN_NAME).toStdString());
    }
    p_event->set_date(pCurrent->text(TREE_COLUMN_DATE).toStdString());
    mpEventWidget->setEvent(p_event);
  }
  else
  {
    mpEventWidget->clearEvent();
  }

  // don't want to have add/delete buttons enabled if we're currently adding
  bool b_buttons_enabled = pCurrent != nullptr && pCurrent != mpAddItem;
  mpAddButton->setEnabled(b_buttons_enabled);
  mpDeleteButton->setEnabled(b_buttons_enabled);
}

void AdminWidget::addClicked(bool checked)
{
  if (mpAddItem != nullptr)
  {
    // discard old thing we were adding (this removes it from the tree)
    delete mpAddItem;
    mpAddItem = nullptr;
  }

  // find the current node
  QTreeWidgetItem* p_item = mpTree->currentItem();
  if (p_item == nullptr)
  {
    return;
  }

  // find the organization of the thing we clicked on
  QTreeWidgetItem* p_org_item = nullptr;
  switch (p_item->type())
  {
    case TREE_ITEM_TYPE_ORGANIZATION:
      AXT_INFO << "Clicked on org";
      p_org_item = p_item;
      break;
    case TREE_ITEM_TYPE_SEASON:
      AXT_INFO << "Clicked on season";
      p_org_item = p_item->parent();
      break;
    case TREE_ITEM_TYPE_EVENT:
      AXT_INFO << "Clicked on event";
      p_org_item = p_item->parent()->parent();
      break;
    default:
      break;
  }

  // add a new item under the current organization
  mpAddItem = new QTreeWidgetItem(p_org_item,  AdminWidget::TREE_ITEM_TYPE_EVENT);
  mpAddItem->setText(TREE_COLUMN_NAME, QString("<new-event>"));
  // use todays date
  mpAddItem->setText(TREE_COLUMN_DATE, QDate::currentDate().toString(Qt::ISODate));

  // select it in the tree so our widgets on the right get updated with the new items data
  mpTree->setCurrentItem(mpAddItem);
  // disable tree until we're done adding
  mpTree->setEnabled(false);
}

void AdminWidget::deleteClicked(bool checked)
{
  // TODO prompt to confirm deletion
  QTreeWidgetItem* p_item = mpTree->currentItem();
  if (p_item == nullptr)
  {
    return;
  }

  // get ID of the current item
  std::int64_t id = p_item->data(TREE_COLUMN_ID, TREE_ROLE_ID).toLongLong();
  // delete from the correct table depending on what is selected
  switch (p_item->type())
  {
    case TREE_ITEM_TYPE_ORGANIZATION:
      AXT_INFO << "Delete org: " << id;
      mpOrganizationModel->destroyById(id);
      break;
    case TREE_ITEM_TYPE_SEASON:
      AXT_INFO << "Delete season: " << id;
      mpSeasonModel->destroyById(id);
      break;
    case TREE_ITEM_TYPE_EVENT:
      AXT_INFO << "Delete event: " << id;
      mpEventModel->destroyById(id);
      break;
    default:
      break;
  }

  // removes this from the tree
  delete p_item;
}

void AdminWidget::eventSaved(const autoxtime::proto::Event& savedEvent)
{
  // copy
  autoxtime::proto::Event event = savedEvent;
  QTreeWidgetItem* p_event_item = mpTree->currentItem();
  if (p_event_item == nullptr)
  {
    return;
  }
  // set the season from the parent item
  QTreeWidgetItem* p_season_item = p_event_item->parent();
  event.set_season_id(p_season_item->data(TREE_COLUMN_ID, TREE_ROLE_ID).toLongLong());

  // set the organization from the grandparent item
  QTreeWidgetItem* p_org_item = p_season_item->parent();
  event.set_organization_id(p_org_item->data(TREE_COLUMN_ID, TREE_ROLE_ID).toLongLong());

  if (mpAddItem)
  {
    // discard old thing we were adding (this removes it from the tree)
    // this will reappear when we get an event about it
    mpAddItem = nullptr;

    // create the event, receive back the message that contains the ID of the newly
    // created even
    autoxtime::db::EventModel::ProtoPtrVec events = mpEventModel->create(event);
    if (events.size() > 1)
    {
      // set the newly created event ID on our item
      std::int64_t event_id = events[0]->event_id();
      p_event_item->setData(TREE_COLUMN_ID, TREE_ROLE_ID, QVariant::fromValue(event_id));

      // add our event to our collections
      mEvents.push_back(events[0]);
      mEventTreeItems[event_id] = p_event_item;
    }
  }
  else
  {
    // get the ID in the item so we can set it here, only need this for updates
    event.set_event_id(p_event_item->data(TREE_COLUMN_ID, TREE_ROLE_ID).toLongLong());
    mpEventModel->update(event);
  }

  // update the tree item with the current info from the form
  p_event_item->setText(TREE_COLUMN_NAME, QString::fromStdString(event.name()));
  p_event_item->setText(TREE_COLUMN_DATE, QString::fromStdString(event.date()));
  p_event_item->setData(TREE_COLUMN_ID, TREE_ROLE_ID, QVariant::fromValue(event.event_id()));

  // re-enable the tree since we're done editing
  mpTree->setEnabled(true);
  mpTree->setCurrentItem(p_event_item);

  // re-enable the add/delete buttons
  mpAddButton->setEnabled(true);
  mpDeleteButton->setEnabled(true);
}

void AdminWidget::eventCancelled()
{
  // remove the currently adding item, this should select a new item
  if (mpAddItem != nullptr)
  {
    // discard old thing we were adding (this removes it from the tree)
    delete mpAddItem;
    mpAddItem = nullptr;
  }

  // remove selection
  mpTree->setCurrentItem(nullptr);

  // re-enable the tree since we're done editing
  mpTree->setEnabled(true);
}

AUTOXTIME_UI_NAMESPACE_END
