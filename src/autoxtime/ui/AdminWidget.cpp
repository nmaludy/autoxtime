#include <autoxtime/ui/AdminWidget.h>

#include <autoxtime/proto/organization.pb.h>
#include <autoxtime/proto/event.pb.h>

#include <algorithm>

#include <QGridLayout>
#include <QLabel>
#include <QTreeWidget>

AUTOXTIME_UI_NAMESPACE_BEG

AdminWidget::AdminWidget(QWidget* pParent)
    : QWidget(pParent),
      mpTree(new QTreeWidget(this)),
      mpOrganizationModel(new autoxtime::db::OrganizationModel(this)),
      mOrganizations(),
      mpEventModel(new autoxtime::db::EventModel(this)),
      mEvents()
{
  QGridLayout* p_layout = new QGridLayout(this);

  // tree
  QStringList headers;
  headers << "Organizations";
  headers << "Date";
  mpTree->setHeaderLabels(headers);
  p_layout->addWidget(mpTree, 0, 0);

  // connect our signals/ slots
  connect(mpOrganizationModel, &autoxtime::db::OrganizationModel::listResult,
          this,                &AdminWidget::setOrganizations);
  connect(mpEventModel, &autoxtime::db::EventModel::listResult,
          this,         &AdminWidget::setEvents);

  // kick off async queries
  mpEventModel->listAsync();
  mpOrganizationModel->listAsync();
}

void AdminWidget::setOrganizations(const autoxtime::db::OrganizationModel::ProtoPtrVec& orgs)
{
  mOrganizations = orgs;
  std::sort(mOrganizations.begin(), mOrganizations.end(),
            [](const autoxtime::db::OrganizationModel::ProtoPtr& pLhs,
               const autoxtime::db::OrganizationModel::ProtoPtr& pRhs) -> bool
            {
              return pLhs->name() < pRhs->name();
            });
  rebuildTree();
}

void AdminWidget::setEvents(const autoxtime::db::EventModel::ProtoPtrVec& events)
{
  mEvents = events;
  std::sort(mEvents.begin(), mEvents.end(),
            [](const autoxtime::db::EventModel::ProtoPtr& pLhs,
               const autoxtime::db::EventModel::ProtoPtr& pRhs) -> bool
            {
              return pLhs->name() < pRhs->name();
            });
  rebuildTree();
}

void AdminWidget::rebuildTree()
{
  // TODO - some smart way of maintaining the tree, only with new additions
  //  will be complex with multiple layers in the tree
  mpTree->clear();

  // TODO how to do this not O(n^2)
  // need a lookup from org id -> list<event>
  for (const autoxtime::db::OrganizationModel::ProtoPtr& p_org : mOrganizations)
  {
    QTreeWidgetItem* p_org_item =
        new QTreeWidgetItem(QStringList(QString::fromStdString(p_org->name())));

    for (const autoxtime::db::EventModel::ProtoPtr& p_event : mEvents)
    {
      if (p_org->organization_id() == p_event->organization_id())
      {
        QStringList cols;
        cols << QString::fromStdString(p_event->name());
        cols << QString::fromStdString(p_event->date());
        new QTreeWidgetItem(p_org_item, cols);
      }
    }
    mpTree->addTopLevelItem(p_org_item);
  }
}

AUTOXTIME_UI_NAMESPACE_END
