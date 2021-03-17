#include <autoxtime/ui/RegistrationWidget.h>

#include <QGridLayout>
#include <QLabel>

AUTOXTIME_UI_NAMESPACE_BEG

RegistrationWidget::RegistrationWidget(QWidget* pParent)
    : QWidget(pParent)
{
  QGridLayout* p_layout = new QGridLayout(this);
  p_layout->addWidget(new QLabel("This will be some registration stuff"), 0, 0);
}

AUTOXTIME_UI_NAMESPACE_END
