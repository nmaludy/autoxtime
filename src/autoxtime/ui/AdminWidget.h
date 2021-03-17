#ifndef AUTOXTIME_UI_ADMINWIDGET
#define AUTOXTIME_UI_ADMINWIDGET

#include <autoxtime/ui/ui.h>
#include <QWidget>

#include <autoxtime/db/EventModel.h>
#include <autoxtime/db/OrganizationModel.h>

class QLabel;
class QTreeWidget;

AUTOXTIME_UI_NAMESPACE_BEG

class AdminWidget : public QWidget
{
  Q_OBJECT

 public:
  AdminWidget(QWidget* pParent = nullptr);

 public slots:
  void setOrganizations(const autoxtime::db::OrganizationModel::ProtoPtrVec& orgs);
  void setEvents(const autoxtime::db::EventModel::ProtoPtrVec& events);

 private:
  void rebuildTree();

  // QLabel* mpLabel;
  QTreeWidget* mpTree;

  autoxtime::db::OrganizationModel* mpOrganizationModel;
  autoxtime::db::OrganizationModel::ProtoPtrVec mOrganizations;

  autoxtime::db::EventModel* mpEventModel;
  autoxtime::db::EventModel::ProtoPtrVec mEvents;
};

AUTOXTIME_UI_NAMESPACE_END

#endif // AUTOXTIME_UI_ADMINWIDGET
