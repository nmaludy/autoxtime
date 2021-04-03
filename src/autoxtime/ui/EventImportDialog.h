#ifndef AUTOXTIME_UI_EVENTIMPORTDIALOG
#define AUTOXTIME_UI_EVENTIMPORTDIALOG

// autoxtime
#include <autoxtime/ui/ui.h>

// Qt
#include <QDialog>

class QComboBox;
class QDateEdit;
class QLineEdit;
class QPushButton;

AUTOXTIME_UI_NAMESPACE_BEG

class EventImportDialog : public QDialog
{
  Q_OBJECT

 public:
  explicit EventImportDialog(std::int64_t eventId,
                             QWidget* pParent = nullptr);

 public slots:
  void browseClicked(bool checked = false);
  void saveClicked(bool checked = false);

 private:
  const std::int64_t mEventId;

  QComboBox* mpFormatComboBox;

  QLineEdit* mpFileLineEdit;
  QPushButton* mpFileBrowseButton;

  QPushButton* mpSaveButton;
  QPushButton* mpCancelButton;
};

AUTOXTIME_UI_NAMESPACE_END

#endif // AUTOXTIME_UI_EVENTIMPORTDIALOG
