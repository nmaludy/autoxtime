#include <autoxtime/timing/TimingStateMachine.h>

#include <autoxtime/codec/ICodec.h>
#include <autoxtime/log/Log.h>

AUTOXTIME_NAMESPACE_BEG

TimingStateMachine::TimingStateMachine(ICodec* pTimersCodec,
                                       QObject* pParent)
    : QObject(pParent)
{
  connect(pTimersCodec, &ICodec::messagesRead,
          this,         &TimingStateMachine::handleMessagesRead);
}

void TimingStateMachine
::handleMessagesRead(const std::vector<std::shared_ptr<google::protobuf::Message>>& msgs)
{
  // TODO
  // - create a message for the timing records (raw)
  // - write timing records to database
  // - create a message for car queue
  // - query/subscribe for queued cars
  //
  // - as timing messages come in, assign them to cars in the queue
  // - create a message to associate timing records -> cars (runs)
  //
  // each car goes from:
  // - Queued (ordered)
  // - Start (triggered by start timer)
  // - Sector #1 (only after start, if we get a sector time before Finish)
  // - Sector #2 (only after sector #1, if we get a sector time before Finish)
  // - Finish (triggered by finish timer, only if started)
  //
  // each state you advance the "first" car in the queue waiting on that state

  AXT_DEBUG << "TimingStateMachine::handleMessagesRead()";
}

AUTOXTIME_NAMESPACE_END
