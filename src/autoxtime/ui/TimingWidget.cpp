#include <autoxtime/ui/TimingWidget.h>

#include <autoxtime/ui/RegistrationTableWidget.h>

#include <QGridLayout>
#include <QLabel>

AUTOXTIME_UI_NAMESPACE_BEG

TimingWidget::TimingWidget(QWidget* pParent)
    : QWidget(pParent),
      mpRegTableWidget(new RegistrationTableWidget(this))
{
  QGridLayout* p_layout = new QGridLayout(this);
  p_layout->addWidget(mpRegTableWidget, 0, 0);
}

AUTOXTIME_UI_NAMESPACE_END
