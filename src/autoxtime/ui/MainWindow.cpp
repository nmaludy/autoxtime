#include <autoxtime/ui/MainWindow.h>

#include <QLabel>

AUTOXTIME_NAMESPACE_BEG

MainWindow::MainWindow(QWidget* pParent)
    : QMainWindow(pParent)
{
  setCentralWidget(new QLabel("autoxtime"));
}

AUTOXTIME_NAMESPACE_END
