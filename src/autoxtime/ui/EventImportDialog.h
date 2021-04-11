#ifndef AUTOXTIME_UI_EVENTIMPORTDIALOG
#define AUTOXTIME_UI_EVENTIMPORTDIALOG

// autoxtime
#include <autoxtime/ui/ui.h>

// Qt
#include <QDialog>

// std
#include <memory>

class QComboBox;
class QDateEdit;
class QLineEdit;
class QPushButton;

namespace autoxtime { namespace proto { class Event; } }

AUTOXTIME_UI_NAMESPACE_BEG

class EventImportDialog : public QDialog
{
  Q_OBJECT

 public:
  explicit EventImportDialog(const std::shared_ptr<autoxtime::proto::Event>& pEvent,
                             QWidget* pParent = nullptr);

 public slots:
  void browseClicked(bool checked = false);
  void saveClicked(bool checked = false);

 private:
  const std::shared_ptr<autoxtime::proto::Event> mpEvent;

  QComboBox* mpFormatComboBox;

  QLineEdit* mpFileLineEdit;
  QPushButton* mpFileBrowseButton;

  QPushButton* mpSaveButton;
  QPushButton* mpCancelButton;
};

AUTOXTIME_UI_NAMESPACE_END

#endif // AUTOXTIME_UI_EVENTIMPORTDIALOG
