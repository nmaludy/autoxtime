#include <autoxtime/ui/MainWindow.h>

#include <autoxtime/ui/AdminWidget.h>
#include <autoxtime/ui/RegistrationWidget.h>

#include <QDockWidget>
#include <QVector>

AUTOXTIME_UI_NAMESPACE_BEG

MainWindow::MainWindow(QWidget* pParent)
    : QMainWindow(pParent),
      mpRegistrationWidget(new RegistrationWidget(this)),
      mpAdminWidget(new AdminWidget(this))
{
  setDockOptions(QMainWindow::AllowTabbedDocks);
  Qt::DockWidgetArea dock_area = Qt::TopDockWidgetArea;
  setTabPosition(Qt::TopDockWidgetArea, QTabWidget::North);

  QVector<QDockWidget*> dock_widgets;
  {
    QDockWidget* p_dock_widget = new QDockWidget("Registration", this);
    p_dock_widget->setWidget(mpRegistrationWidget);
    addDockWidget(dock_area, p_dock_widget);
    dock_widgets << p_dock_widget;
  }

  {
    QDockWidget* p_dock_widget = new QDockWidget("Admin", this);
    p_dock_widget->setWidget(mpAdminWidget);
    addDockWidget(dock_area, p_dock_widget);
    dock_widgets << p_dock_widget;
  }

  for (int i = 1; i < dock_widgets.size(); ++i)
  {
    tabifyDockWidget(dock_widgets.at(i-1), dock_widgets.at(i));
  }
}

AUTOXTIME_UI_NAMESPACE_END
