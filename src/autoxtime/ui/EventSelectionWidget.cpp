#include <autoxtime/ui/EventSelectionWidget.h>

#include <autoxtime/db/EventModel.h>
#include <autoxtime/log/Log.h>
#include <autoxtime/proto/event.pb.h>

#include <QComboBox>
#include <QGridLayout>
#include <QLabel>

AUTOXTIME_UI_NAMESPACE_BEG

EventSelectionWidget::EventSelectionWidget(QWidget* pParent)
    : QWidget(pParent),
      mpEventModel(new autoxtime::db::EventModel(this)),
      mpEventComboBox(new QComboBox(this))
{
  QGridLayout* p_layout = new QGridLayout(this);
  p_layout->setContentsMargins(0, 0, 0, 0);

  // label
  {
    QLabel* p_label = new QLabel("Event");
    p_label->setAlignment(p_label->alignment() | Qt::AlignRight);
    p_layout->addWidget(p_label, 0, 0);
  }

  // combo
  {
    // might want to change this later to not be alphabetical, maybe order by date?
    mpEventComboBox->setInsertPolicy(QComboBox::InsertAlphabetically);

    connect(mpEventComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,            &EventSelectionWidget::eventComboIndexChanged);
    connect(mpEventModel, &autoxtime::db::EventModel::listResult,
            this,         &EventSelectionWidget::setEvents);
    mpEventModel->listAsync();
    p_layout->addWidget(mpEventComboBox, 0, 1, 1, -1);
  }
}

void EventSelectionWidget::setEvents(const autoxtime::db::EventModel::ProtoPtrVec& events)
{
  mpEventComboBox->clear();
  for (const std::shared_ptr<autoxtime::proto::Event>& p_event : events)
  {
    mpEventComboBox->addItem(QString::fromStdString(p_event->name()),
                             QVariant::fromValue(p_event->event_id()));
  }
}

void EventSelectionWidget::eventComboIndexChanged(int index)
{
  if (index == -1)
  {
    AXT_DEBUG << "Event combo box reset or cleared";
    return;
  }
  // get current event ID and use it for binding in SQL queries
  std::int64_t event_id = mpEventComboBox->currentData().toLongLong();
  emit eventChanged(event_id);
}

AUTOXTIME_UI_NAMESPACE_END
