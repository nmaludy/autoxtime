#ifndef AUTOXTIME_UI_REGISTRATIONWIDGET
#define AUTOXTIME_UI_REGISTRATIONWIDGET

#include <autoxtime/ui/ui.h>
#include <QWidget>

AUTOXTIME_UI_NAMESPACE_BEG

class RegistrationWidget : public QWidget
{
  Q_OBJECT

 public:
  explicit RegistrationWidget(QWidget* pParent = nullptr);
};

AUTOXTIME_UI_NAMESPACE_END

#endif // AUTOXTIME_UI_REGISTRATIONWIDGET
