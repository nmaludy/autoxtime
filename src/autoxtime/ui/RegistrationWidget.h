#ifndef AUTOXTIME_UI_REGISTRATIONWIDGET
#define AUTOXTIME_UI_REGISTRATIONWIDGET

#include <autoxtime/ui/ui.h>
#include <QWidget>

class QComboBox;
class QToolButton;
class QSplitter;
class QStatusBar;

namespace autoxtime { namespace ui { class RegistrationTableWidget; } }

AUTOXTIME_UI_NAMESPACE_BEG

class RegistrationWidget : public QWidget
{
  Q_OBJECT

 public:
  explicit RegistrationWidget(QWidget* pParent = nullptr);

 public slots:
  void splitterClicked();
  void splitterMoved(int pos, int index);
  void eventChanged(std::int64_t eventId);

 private:
  void updateStatusBar(std::uint64_t numEntries,
                       std::uint64_t numCheckedIn);

  // splitter
  QSplitter* mpSplitter;
  QToolButton* mpSplitterButton;

  // table
  RegistrationTableWidget* mpTableWidget;

  // status bar
  QStatusBar* mpStatusBar;
};

AUTOXTIME_UI_NAMESPACE_END

#endif // AUTOXTIME_UI_REGISTRATIONWIDGET
