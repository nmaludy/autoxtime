#include <autoxtime/ui/MainWindow.h>

#include <autoxtime/ui/AdminWidget.h>
#include <autoxtime/ui/RegistrationWidget.h>
#include <autoxtime/ui/TimingWidget.h>

#include <QDockWidget>
#include <QGuiApplication>
#include <QLabel>
#include <QMenuBar>
#include <QVector>

AUTOXTIME_UI_NAMESPACE_BEG

MainWindow::MainWindow(QWidget* pParent)
    : QMainWindow(pParent),
      mpTabWidget(new QTabWidget(this)),
      mpAdminWidget(new AdminWidget(this)),
      mpRegistrationWidget(new RegistrationWidget(this)),
      mpTimingWidget(new TimingWidget(this))
{
  QIcon icon("../img/autoxtime_icon.png");
  setWindowIcon(icon);

  // menu
  QMenuBar* p_menu_bar = menuBar();
  QLabel* p_icon_label = new QLabel(this);
  p_icon_label->setPixmap(icon.pixmap(32, 32));
  p_menu_bar->setCornerWidget(p_icon_label);

  // menu - file
  {
    QMenu* p_file_menu = menuBar()->addMenu("File");
    QAction* p_quit_action = p_file_menu->addAction("Quit");
    connect(p_quit_action, &QAction::triggered,
            qGuiApp,       &QGuiApplication::quit);
  }

  // // dock
  // setDockOptions(QMainWindow::AllowTabbedDocks);
  // Qt::DockWidgetArea dock_area = Qt::TopDockWidgetArea;
  // setTabPosition(Qt::TopDockWidgetArea, QTabWidget::North);

  // QVector<QDockWidget*> dock_widgets;
  // {
  //   QDockWidget* p_dock_widget = new QDockWidget("Admin", this);
  //   p_dock_widget->setWidget(mpAdminWidget);
  //   addDockWidget(dock_area, p_dock_widget);
  //   dock_widgets << p_dock_widget;
  // }

  // {
  //   QDockWidget* p_dock_widget = new QDockWidget("Registration", this);
  //   p_dock_widget->setWidget(mpRegistrationWidget);
  //   addDockWidget(dock_area, p_dock_widget);
  //   dock_widgets << p_dock_widget;
  // }

  // // TODO figure out how to select a particular tabbified widget
  // for (int i = 1; i < dock_widgets.size(); ++i)
  // {
  //   tabifyDockWidget(dock_widgets.at(i-1), dock_widgets.at(i));
  // }

  // tabs - use our own tab widget
  {
    mpTabWidget->addTab(mpAdminWidget, "Admin");
    mpTabWidget->addTab(mpRegistrationWidget, "Registration");
    mpTabWidget->addTab(mpTimingWidget, "Timing");
    mpTabWidget->setCurrentIndex(2);
    setCentralWidget(mpTabWidget);
  }

  // TODO create a "View" menu that allows toggling the tabs on/off
  // if only one tab is selected, maximize it?
}

AUTOXTIME_UI_NAMESPACE_END
