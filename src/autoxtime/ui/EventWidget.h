#ifndef AUTOXTIME_UI_EVENTWIDGET
#define AUTOXTIME_UI_EVENTWIDGET

// autoxtime
#include <autoxtime/ui/ui.h>
#include <autoxtime/db/EventModel.h>

// Qt
#include <QWidget>

class QDateEdit;
class QLineEdit;
class QPushButton;

AUTOXTIME_UI_NAMESPACE_BEG

class EventWidget : public QWidget
{
  Q_OBJECT

 public:
  explicit EventWidget(QWidget* pParent = nullptr);

  std::string name() const;
  void setName(const std::string& name);

  QDate date() const;
  std::string dateStr() const;
  void setDate(const QDate& date);
  void setDateStr(const std::string& date);

 public slots:
  void setEvent(autoxtime::db::EventModel::ProtoPtr pEvent);
  void setEvent(const QString& name,
                const QString& date);
  void clearEvent();
  void saveClicked(bool checked = false);
  void cancelClicked(bool checked = false);
  void importClicked(bool checked = false);

 signals:
  void eventSaved(const autoxtime::proto::Event& savedEvent);
  void eventCancelled();

 private:
  autoxtime::db::EventModel::ProtoPtr mpEvent;

  QLineEdit* mpEventNameLineEdit;
  QDateEdit* mpEventDateEdit;
  QPushButton* mpEventImportButton;
  QPushButton* mpEventSaveButton;
  QPushButton* mpEventCancelButton;
};

AUTOXTIME_UI_NAMESPACE_END

#endif // AUTOXTIME_UI_EVENTWIDGET
