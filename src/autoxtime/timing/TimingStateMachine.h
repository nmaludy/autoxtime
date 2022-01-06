#ifndef AUTOXTIME_TIMING_TIMINGSTATEMACHINE
#define AUTOXTIME_TIMING_TIMINGSTATEMACHINE

#include <autoxtime/autoxtime.h>

// Qt
#include <QObject>

// std
#include <memory>
#include <vector>

namespace autoxtime { class ICodec; }
namespace google { namespace protobuf { class Message; } }

AUTOXTIME_NAMESPACE_BEG

class TimingStateMachine : public QObject
{
  Q_OBJECT

 public:
  explicit TimingStateMachine(ICodec* pTimersCodec, QObject* pParent = nullptr);
  virtual ~TimingStateMachine() = default;

 public slots:
  void handleMessagesRead(const std::vector<std::shared_ptr<google::protobuf::Message>>& msgs);
};

AUTOXTIME_NAMESPACE_END

#endif // AUTOXTIME_TIMING_TIMINGSTATEMACHINE
