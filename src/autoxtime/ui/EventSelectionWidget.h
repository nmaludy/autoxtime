#ifndef AUTOXTIME_UI_EVENTSELECTIONWIDGET
#define AUTOXTIME_UI_EVENTSELECTIONWIDGET

#include <autoxtime/ui/ui.h>
#include <QWidget>

class QComboBox;

namespace autoxtime { namespace db { class EventModel; } }
namespace autoxtime { namespace proto { class Event; } }

AUTOXTIME_UI_NAMESPACE_BEG

class EventSelectionWidget : public QWidget
{
  Q_OBJECT

 public:
  explicit EventSelectionWidget(QWidget* pParent = nullptr);

 signals:
  void eventChanged(std::int64_t eventId);

 public slots:
  void setEvents(const std::vector<std::shared_ptr<autoxtime::proto::Event>>& events);
  void eventComboIndexChanged(int index);

 private:
  // models
  autoxtime::db::EventModel* mpEventModel;

  // combo box
  QComboBox* mpEventComboBox;
};

AUTOXTIME_UI_NAMESPACE_END

#endif // AUTOXTIME_UI_EVENTSELECTION
