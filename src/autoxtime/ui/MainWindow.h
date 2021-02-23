#ifndef AUTOXTIME_UI_MAINWINDOW
#define AUTOXTIME_UI_MAINWINDOW

#include <QObject>

class ITransport;

class MainWindow : public QObject
{
  Q_OBJECT

 public:
  MainWindow(QObject* pParent = nullptr);

 private:
  ITransport* mpTransport;
  QByteArray mDataBuffer;
};

#endif // AUTOXTIME_UI_MAINWINDOW
