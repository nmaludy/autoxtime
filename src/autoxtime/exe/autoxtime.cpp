#include <autoxtime/autoxtime.h>

// autoxtime
#include <autoxtime/exe/AppCommon.h>
#include <autoxtime/log/Log.h>
#include <autoxtime/db/DbListener.h>
#include <autoxtime/ui/MainWindow.h>
#include <autoxtime/codec/MotorsportRegCodec.h>

// Qt
#include <QApplication>

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  autoxtime::AppCommon::init(&app, "autoxtime");

  // create and launch our main window
  autoxtime::ui::MainWindow w;
  w.show();
  int res = app.exec();

  // cleanly shutdown the DbListener thread
  autoxtime::db::DbListener::instance().quit();
  autoxtime::db::DbListener::instance().wait();

  return res;
}
