#include <autoxtime/ui/TimingWidget.h>

#include <QGridLayout>
#include <QLabel>

AUTOXTIME_UI_NAMESPACE_BEG

TimingWidget::TimingWidget(QWidget* pParent)
    : QWidget(pParent)
{
  QGridLayout* p_layout = new QGridLayout(this);
  p_layout->addWidget(new QLabel("Timing"), 0, 0);
}

AUTOXTIME_UI_NAMESPACE_END
