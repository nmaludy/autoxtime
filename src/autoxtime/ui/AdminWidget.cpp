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

AdminWidget::AdminWidget(QWidget* pParent)
    : QWidget(pParent),
      mState(AdminWidget::STATE_DEFAULT),
      mpTree(new QTreeWidget(this)),
      mpAddButton(new QPushButton("add", this)),
      mpDeleteButton(new QPushButton("delete", this)),
      mpEventFrame(new QFrame(this)),
      mpEventNameLineEdit(new QLineEdit(this)),
      mpEventDateEdit(new QDateEdit(this)),
      mpEventSaveButton(new QPushButton("save", this)),
      mpEventCancelButton(new QPushButton("cancel", this)),
      mpAddItem(nullptr),
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
    mpEventDateEdit->setCalendarPopup(true);
    mpEventDateEdit->setDate(QDate::currentDate());

    QGridLayout* p_evemt_layout = new QGridLayout(mpEventFrame);
    p_evemt_layout->addWidget(new QLabel("Name", this), 0, 0);
    p_evemt_layout->addWidget(mpEventNameLineEdit, 0, 1, 1, -1);
    p_evemt_layout->addWidget(new QLabel("Date", this), 1, 0);
    p_evemt_layout->addWidget(mpEventDateEdit, 1, 1, 1, -1);
    p_evemt_layout->addWidget(mpEventSaveButton, 3, 0);
    p_evemt_layout->addWidget(mpEventCancelButton, 3, 1, 1, -1);
    p_evemt_layout->setRowStretch(2, 1);

    // add event editor to main layout
    p_layout->addWidget(mpEventFrame, 0, 1);

    // disable until we click on an event
    mpEventFrame->setEnabled(false);
    mpEventNameLineEdit->setEnabled(false);
    mpEventDateEdit->setEnabled(false);
    mpEventSaveButton->setEnabled(false);
    mpEventCancelButton->setEnabled(false);

    // capture save/cancel events
    connect(mpEventSaveButton, &QPushButton::clicked,
            this,              &AdminWidget::eventSaveClicked);
    connect(mpEventCancelButton, &QPushButton::clicked,
            this,                &AdminWidget::eventCancelClicked);
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
    rebuildTreeOrganization(p_org);
  }

  for (const autoxtime::db::EventModel::ProtoPtr& p_event : mEvents)
  {
    rebuildTreeEvent(p_event);
  }
}

void AdminWidget::rebuildTreeOrganization(const autoxtime::db::OrganizationModel::ProtoPtr& pOrg)
{
  // do we know about this organization yet or not?
  std::int32_t org_id = pOrg->organization_id();
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
  p_org_item->setText(TREE_COLUMN_NAME, QString::fromStdString(pOrg->name()));
  p_org_item->setData(TREE_COLUMN_ID, TREE_ROLE_ID, QVariant::fromValue(pOrg->organization_id()));
}

void AdminWidget::rebuildTreeEvent(const autoxtime::db::EventModel::ProtoPtr& pEvent)
{
  // do we know about this event yet or not?
  std::int32_t event_id = pEvent->event_id();
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
    std::int32_t org_id = pEvent->organization_id();
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
      return;
    }

    // creates a new item and adds it as a child under the organization
    p_event_item = new QTreeWidgetItem(p_org_item, AdminWidget::TREE_ITEM_TYPE_EVENT);
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
  qInfo().nospace() << "Selected: " << pCurrent;
  bool b_event_item = pCurrent && pCurrent->type() == TREE_ITEM_TYPE_EVENT;
  if (b_event_item)
  {
    // don't put the fake name in the text box
    if (pCurrent == mpAddItem)
    {
      mpEventNameLineEdit->clear();
    }
    else
    {
      mpEventNameLineEdit->setText(pCurrent->text(TREE_COLUMN_NAME));
    }
    QDate d = QDate::fromString(pCurrent->text(TREE_COLUMN_DATE), Qt::ISODate);
    mpEventDateEdit->setDate(d);
  }

  mpEventFrame->setEnabled(b_event_item);
  mpEventNameLineEdit->setEnabled(b_event_item);
  mpEventDateEdit->setEnabled(b_event_item);
  mpEventSaveButton->setEnabled(b_event_item);
  mpEventCancelButton->setEnabled(b_event_item);

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
      qInfo().nospace() << "Clicked on org";
      p_org_item = p_item;
      break;
    case TREE_ITEM_TYPE_EVENT:
      qInfo().nospace() << "Clicked on event";
      p_org_item = p_item->parent();
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
  int id = p_item->data(TREE_COLUMN_ID, TREE_ROLE_ID).toInt();
  // delete from the correct table depending on what is selected
  switch (p_item->type())
  {
    case TREE_ITEM_TYPE_ORGANIZATION:
      qInfo().nospace() << "Delete org: " << id;
      mpOrganizationModel->destroyById(id);
      break;
    case TREE_ITEM_TYPE_EVENT:
      qInfo().nospace() << "Delete event: " << id;
      mpEventModel->destroyById(id);
      break;
    default:
      break;
  }

  // removes this from the tree
  delete p_item;
}

void AdminWidget::eventSaveClicked(bool checked)
{
  autoxtime::proto::Event event;
  event.set_name(mpEventNameLineEdit->text().toStdString());
  event.set_date(mpEventDateEdit->date().toString(Qt::ISODate).toStdString());
  QTreeWidgetItem* p_item = mpTree->currentItem();
  if (p_item == nullptr)
  {
    return;
  }
  // set the organization from the parent item
  QTreeWidgetItem* p_org_item = p_item->parent();
  event.set_organization_id(p_org_item->data(TREE_COLUMN_ID, TREE_ROLE_ID).toInt());

  if (mpAddItem)
  {
    // discard old thing we were adding (this removes it from the tree)
    delete mpAddItem;
    mpAddItem = nullptr;

    mpEventModel->create(event);
  }
  else
  {
    // get the ID in the item so we can set it here, only need this for updates
    event.set_event_id(p_item->data(TREE_COLUMN_ID, TREE_ROLE_ID).toInt());
    mpEventModel->update(event);
  }

  // re-enable the tree since we're done editing
  mpTree->setEnabled(true);

}

void AdminWidget::eventCancelClicked(bool checked)
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
  // reset name edit
  mpEventNameLineEdit->clear();
  // reset date edit
  mpEventDateEdit->setDate(QDate::currentDate());

  // re-enable the tree since we're done editing
  mpTree->setEnabled(true);
}

AUTOXTIME_UI_NAMESPACE_END
