#ifndef AUTOXTIME_UI_TIMINGWIDGET
#define AUTOXTIME_UI_TIMINGWIDGET

#include <autoxtime/ui/ui.h>
#include <QWidget>

class QPushButton;

namespace autoxtime { namespace db { class RunModel; } }
namespace autoxtime { namespace ui { class RegistrationTableWidget; } }
namespace autoxtime { namespace ui { class RunTableWidget; } }

AUTOXTIME_UI_NAMESPACE_BEG

class TimingWidget : public QWidget
{
  Q_OBJECT

 public:
  explicit TimingWidget(QWidget* pParent = nullptr);

 public slots:
  void eventChanged(std::int64_t eventId);
  void addClicked(bool checked = false);

 private:
  autoxtime::db::RunModel* mpRunModel;
  RunTableWidget* mpRunTableWidget;
  RegistrationTableWidget* mpRegistrationTableWidget;
  QPushButton* mpAddButton;
  std::int64_t mEventId;
};

AUTOXTIME_UI_NAMESPACE_END

#endif // AUTOXTIME_UI_TIMINGWIDGET
