#ifndef AUTOXTIME_UI_MAINWINDOW
#define AUTOXTIME_UI_MAINWINDOW

#include <autoxtime/ui/ui.h>
#include <QMainWindow>

class QTabWidget;

AUTOXTIME_UI_NAMESPACE_BEG

class AdminWidget;
class RegistrationWidget;

class MainWindow : public QMainWindow
{
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* pParent = nullptr);

 private:
  QTabWidget* mpTabWidget;
  RegistrationWidget* mpRegistrationWidget;
  AdminWidget* mpAdminWidget;
};

AUTOXTIME_UI_NAMESPACE_END

#endif // AUTOXTIME_UI_MAINWINDOW
