#include <autoxtime/ui/TimingWidget.h>

#include <autoxtime/db/RunModel.h>
#include <autoxtime/log/Log.h>
#include <autoxtime/proto/run.pb.h>
#include <autoxtime/ui/MultiSortFilterProxyModel.h>
#include <autoxtime/ui/RegistrationTableModel.h>
#include <autoxtime/ui/RegistrationTableWidget.h>
#include <autoxtime/ui/RunTableModel.h>
#include <autoxtime/ui/RunTableWidget.h>

#include <QGridLayout>
#include <QItemSelectionModel>
#include <QLabel>
#include <QPushButton>
#include <QTableView>

#include <unordered_map>

AUTOXTIME_UI_NAMESPACE_BEG

TimingWidget::TimingWidget(QWidget* pParent)
    : QWidget(pParent),
      mpRunModel(new autoxtime::db::RunModel(this)),
      mpRunTableWidget(new RunTableWidget(this)),
      mpRegistrationTableWidget(new RegistrationTableWidget(this)),
      mpAddButton(new QPushButton("Add", this)),
      mEventId(-1)
{
  QGridLayout* p_layout = new QGridLayout(this);
  p_layout->addWidget(mpRunTableWidget, 0, 0);

  {
    QFrame* p_frame = new QFrame(this);
    QGridLayout* p_frame_layout = new QGridLayout(p_frame);

    connect(mpAddButton, &QPushButton::clicked,
          this,          &TimingWidget::addClicked);

    p_frame_layout->addWidget(mpRegistrationTableWidget, 0, 0);
    p_frame_layout->addWidget(mpAddButton, 1, 0);
    p_layout->addWidget(p_frame, 0, 1);
  }
}

void TimingWidget::eventChanged(std::int64_t eventId)
{
  mEventId = eventId;
  mpRunTableWidget->setEventId(eventId);
  mpRegistrationTableWidget->setEventId(eventId);
}

void TimingWidget::addClicked(bool checked)
{
  if (mEventId < 0)
  {
    AXT_ERROR << "Timing Widget Event ID < 0: " << mEventId;
    return;
  }

  AXT_DEBUG << "Add clicked";
  QItemSelectionModel* p_select = mpRegistrationTableWidget->view()->selectionModel();
  if (p_select->hasSelection())
  {
    AXT_DEBUG << "has selection";
    QModelIndexList list = p_select->selectedIndexes();
    AXT_DEBUG << "selection size = " << list.size();
    std::unordered_map<int, QModelIndex> row_idxs;
    for (const QModelIndex& proxy_idx :  p_select->selectedIndexes())
    {
      if (row_idxs.find(proxy_idx.row()) == row_idxs.end())
      {
        row_idxs[proxy_idx.row()] = proxy_idx;
      }
    }
    AXT_DEBUG << "row_idxs size = " << row_idxs.size();

    for (const std::pair<const int, QModelIndex>& row_proxy_idx : row_idxs)
    {
      QModelIndex data_idx =
          mpRegistrationTableWidget->proxyModel()->mapToSource(row_proxy_idx.second);
      std::pair<std::int64_t, std::int64_t> car_driver_id =
          mpRegistrationTableWidget->model()->indexCarDriverIds(data_idx);

      // TODO make run
      AXT_DEBUG << "selected car_id=" << car_driver_id.first
                << " driver_id=" << car_driver_id.second;

      if (car_driver_id.first == -1 && car_driver_id.second == -1)
      {
        AXT_DEBUG << "selected invalid car/driver row somehow";
        continue;
      }

      autoxtime::proto::Run run;
      run.set_event_id(mEventId);
      run.set_car_id(car_driver_id.first);
      run.set_driver_id(car_driver_id.second);
      // get previous ID from run model
      QModelIndex run_proxy_idx = mpRunTableWidget->proxyModel()->index(
          mpRunTableWidget->proxyModel()->rowCount() - 1, 0);
      QModelIndex run_idx =
          mpRunTableWidget->proxyModel()->mapToSource(run_proxy_idx);
      std::int64_t prev_run_id = mpRunTableWidget->model()->indexRunId(run_idx);
      if (prev_run_id != -1)
      {
        AXT_DEBUG << "found previous run id: " << prev_run_id;
        run.set_previous_run_id(prev_run_id);
      }
      else
      {
        AXT_DEBUG << "no previous run";
      }
      mpRunModel->create(run);
    }
  }
  else
  {
    AXT_DEBUG << "no selection";
  }
}

AUTOXTIME_UI_NAMESPACE_END
