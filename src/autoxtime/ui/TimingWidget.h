#ifndef AUTOXTIME_UI_TIMINGWIDGET
#define AUTOXTIME_UI_TIMINGWIDGET

#include <autoxtime/ui/ui.h>
#include <QWidget>


AUTOXTIME_UI_NAMESPACE_BEG

class RegistrationTableWidget;

class TimingWidget : public QWidget
{
  Q_OBJECT

 public:
  explicit TimingWidget(QWidget* pParent = nullptr);

 public slots:
  void eventChanged(std::int64_t eventId);

 private:
  RegistrationTableWidget* mpRegTableWidget;
};

AUTOXTIME_UI_NAMESPACE_END

#endif // AUTOXTIME_UI_TIMINGWIDGET
