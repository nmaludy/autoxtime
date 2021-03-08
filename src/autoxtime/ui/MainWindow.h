#ifndef AUTOXTIME_UI_MAINWINDOW
#define AUTOXTIME_UI_MAINWINDOW

#include <autoxtime/autoxtime.h>
#include <QMainWindow>

AUTOXTIME_NAMESPACE_BEG

class MainWindow : public QMainWindow
{
  Q_OBJECT

 public:
  MainWindow(QWidget* pParent = nullptr);
};

AUTOXTIME_NAMESPACE_END

#endif // AUTOXTIME_UI_MAINWINDOW
