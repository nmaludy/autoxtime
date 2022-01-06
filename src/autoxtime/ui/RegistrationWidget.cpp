#include <autoxtime/ui/RegistrationWidget.h>

#include <autoxtime/log/Log.h>
#include <autoxtime/ui/RegistrationTableWidget.h>

#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSplitter>
#include <QStatusBar>
#include <QToolButton>

AUTOXTIME_UI_NAMESPACE_BEG

RegistrationWidget::RegistrationWidget(QWidget* pParent)
    : QWidget(pParent),
      mpSplitter(new QSplitter(this)),
      mpSplitterButton(new QToolButton(this)),
      mpTableWidget(new RegistrationTableWidget(this)),
      mpStatusBar(new QStatusBar(this))
{
  QGridLayout* p_layout = new QGridLayout(this);

  // table
  {
    mpSplitter->addWidget(mpTableWidget);
    connect(mpTableWidget, &RegistrationTableWidget::numEntriesChanged,
            this,          &RegistrationWidget::updateStatusBar);

    // don't let the table be hidden
    mpSplitter->setCollapsible(mpSplitter->count() - 1, false);
  }

  // TODO editor widget for registration

  // splitter
  {
    // need to add widget before getting the handle
    // it is only created once a widget is added
    mpSplitter->addWidget(new QLabel("hello world"));
    QSplitterHandle* p_handle = mpSplitter->handle(1);

    QGridLayout* p_handle_layout = new QGridLayout(p_handle);

    // frame/line
    QFrame* p_handle_frame = new QFrame(p_handle);
    p_handle_frame->setFrameShape(QFrame::Box);
    p_handle_frame->setFrameShadow(QFrame::Raised);

    p_handle_layout->addWidget(p_handle_frame, 0, 0, 1, 1);
    p_handle_layout->setRowStretch(0, 1);

    // button
    mpSplitterButton->setStyleSheet("QToolButton { border: none; }");
    mpSplitterButton->setArrowType(Qt::ArrowType::LeftArrow);
    mpSplitter->setHandleWidth(24);
    p_handle_layout->addWidget(mpSplitterButton, 1, 0, 1, 1);

    p_handle->setLayout(p_handle_layout);

    // show table, hide editor
    mpSplitter->setSizes({1, 0});

    p_layout->addWidget(mpSplitter, 1, 0, 1, -1);

    connect(mpSplitterButton, &QPushButton::clicked,
            this,             &RegistrationWidget::splitterClicked);
    connect(mpSplitter, &QSplitter::splitterMoved,
            this,       &RegistrationWidget::splitterMoved);
  }

  // status bar
  {
    mpStatusBar->setSizeGripEnabled(false);
    p_layout->addWidget(mpStatusBar, 2, 0, 1, -1);
  }

  p_layout->setRowStretch(1, 1);
  p_layout->setColumnStretch(1, 1);
}

void RegistrationWidget::eventChanged(std::int64_t eventId)
{
  mpTableWidget->setEventId(eventId);
}

void RegistrationWidget::updateStatusBar(std::uint64_t numEntries,
                                         std::uint64_t numCheckedIn)
{
  mpStatusBar->showMessage(QString("Entries: %1    ✓ In: %2    Missing: %3")
                           .arg(numEntries)
                           .arg(numCheckedIn)
                           .arg(numEntries - numCheckedIn));
}

void RegistrationWidget::splitterClicked()
{
  QList<int> sizes = mpSplitter->sizes();
  if (sizes.last() == 0)
  {
    // right widget will be shown, make arrow to the right so it
    // can collapse
    sizes.last() = 1;
    mpSplitterButton->setArrowType(Qt::ArrowType::RightArrow);
  }
  else
  {
    // right widget will be hidden, make arrow to the left so it
    // can expand
    sizes.last() = 0;
    mpSplitterButton->setArrowType(Qt::ArrowType::LeftArrow);
  }
  mpSplitter->setSizes(sizes);
}

void RegistrationWidget::splitterMoved(int pos, int index)
{
  QList<int> sizes = mpSplitter->sizes();
  // are we at the right edge? if so, expand to the left
  if (sizes.last() == 0)
  {
    mpSplitterButton->setArrowType(Qt::ArrowType::LeftArrow);
  }
  else
  {
    mpSplitterButton->setArrowType(Qt::ArrowType::RightArrow);
  }
}

AUTOXTIME_UI_NAMESPACE_END
