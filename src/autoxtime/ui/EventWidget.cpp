#include <autoxtime/ui/EventWidget.h>

// autoxtime
#include <autoxtime/db/EventModel.h>
#include <autoxtime/log/Log.h>
#include <autoxtime/proto/event.pb.h>
#include <autoxtime/ui/EventImportDialog.h>

// Qt
#include <QDateEdit>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

AUTOXTIME_UI_NAMESPACE_BEG

EventWidget::EventWidget(QWidget* pParent)
    : QWidget(pParent),
      mpEvent(),
      mpEventNameLineEdit(new QLineEdit(this)),
      mpEventDateEdit(new QDateEdit(this)),
      mpEventImportButton(new QPushButton("Import MSR", this)),
      mpEventSaveButton(new QPushButton("Save", this)),
      mpEventCancelButton(new QPushButton("Cancel", this))
{
  mpEventDateEdit->setCalendarPopup(true);
  mpEventDateEdit->setDate(QDate::currentDate());

  QGridLayout* p_evemt_layout = new QGridLayout(this);
  // row 0
  p_evemt_layout->addWidget(new QLabel("Name", this), 0, 0);
  p_evemt_layout->addWidget(mpEventNameLineEdit, 0, 1, 1, -1);
  // row 1
  p_evemt_layout->addWidget(new QLabel("Date", this), 1, 0);
  p_evemt_layout->addWidget(mpEventDateEdit, 1, 1, 1, -1);
  // row 2
  p_evemt_layout->addWidget(mpEventImportButton, 2, 1, 1, -1);
  // row 3 = empty, so it can consume all of the space when stretched
  p_evemt_layout->setRowStretch(3, 1);
  // row 4
  p_evemt_layout->addWidget(mpEventSaveButton, 4, 0);
  // so it can consume the wrest of the space
  p_evemt_layout->setColumnStretch(1, 1);
  p_evemt_layout->addWidget(mpEventCancelButton, 4, 2);

  // disable until we click on an event
  setEnabled(false);
  mpEventNameLineEdit->setEnabled(false);
  mpEventDateEdit->setEnabled(false);
  mpEventImportButton->setEnabled(false);
  mpEventSaveButton->setEnabled(false);
  mpEventCancelButton->setEnabled(false);

  // capture save/cancel events
  connect(mpEventSaveButton, &QPushButton::clicked,
          this,              &EventWidget::saveClicked);
  connect(mpEventCancelButton, &QPushButton::clicked,
          this,                &EventWidget::cancelClicked);
  connect(mpEventImportButton, &QPushButton::clicked,
          this,                &EventWidget::importClicked);
}

void EventWidget::setEvent(autoxtime::db::EventModel::ProtoPtr pEvent)
{
  mpEvent = pEvent;
  bool b_event_item = mpEvent != nullptr;
  if (b_event_item)
  {
    // don't put the fake name in the text box
    if (pEvent->has_name())
    {
      mpEventNameLineEdit->setText(QString::fromStdString(pEvent->name()));
    }
    else
    {
      mpEventNameLineEdit->clear();
    }

    if (pEvent->has_date())
    {
      QDate d = QDate::fromString(QString::fromStdString(pEvent->date()),
                                  Qt::ISODate);
      mpEventDateEdit->setDate(d);
    }
  }

  setEnabled(b_event_item);
  mpEventNameLineEdit->setEnabled(b_event_item);
  mpEventDateEdit->setEnabled(b_event_item);
  mpEventImportButton->setEnabled(b_event_item);
  mpEventSaveButton->setEnabled(b_event_item);
  mpEventCancelButton->setEnabled(b_event_item);
}

void EventWidget::setEvent(const QString& name, const QString& date)
{
  autoxtime::db::EventModel::ProtoPtr p_event =
      std::make_shared<autoxtime::proto::Event>();
  if (!name.isEmpty())
  {
    p_event->set_name(name.toStdString());
  }
  if (!date.isEmpty())
  {
    p_event->set_date(date.toStdString());
  }
  setEvent(p_event);
}

void EventWidget::clearEvent()
{
  setEvent(autoxtime::db::EventModel::ProtoPtr());
}

void EventWidget::saveClicked(bool checked)
{
  autoxtime::proto::Event event;
  event.set_name(mpEventNameLineEdit->text().toStdString());
  event.set_date(mpEventDateEdit->date().toString(Qt::ISODate).toStdString());

  emit eventSaved(event);
}

void EventWidget::cancelClicked(bool checked)
{
  // reset name edit
  mpEventNameLineEdit->clear();
  // reset date edit
  mpEventDateEdit->setDate(QDate::currentDate());

  emit eventCancelled();
}

void EventWidget::importClicked(bool checked)
{
  if (mpEvent)
  {
    EventImportDialog dialog(mpEvent, this);
    dialog.exec();
  }
}

AUTOXTIME_UI_NAMESPACE_END
