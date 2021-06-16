#ifndef AUTOXTIME_CODEC_FARMTEKCODEC
#define AUTOXTIME_CODEC_FARMTEKCODEC

#include <autoxtime/autoxtime.h>
#include <autoxtime/codec/ICodec.h>

// Qt
#include <qbytearray.h>              // for QByteArray
#include <qobjectdefs.h>             // for Q_OBJECT
#include <qstring.h>                 // for QString

// std
#include <memory>                    // for shared_ptr
#include <vector>                    // for vector

class QObject;
namespace autoxtime { class ITransport; }
namespace google { namespace protobuf { class Message; } }

AUTOXTIME_NAMESPACE_BEG

class FarmtekCodec : public ICodec
{
  Q_OBJECT

 public:
  explicit FarmtekCodec(ITransport* pTransport, QObject* pParent = nullptr);
  virtual ~FarmtekCodec() = default;

  virtual bool decodeData(const QByteArray& data,
                          std::vector<std::shared_ptr<google::protobuf::Message>>& rMsgs) override;

 private:
  // 13 byte fixed format message
  // First character is either B or R (although some modes of the Polaris unit may also give you a S and F, Start finish)
  // 2 signals are sent when an object passes completely through a light beam.
  // The beam break timestamp of the is sent with "B"
  // then the beam restore timestamp is sent with "R"
  // The 2nd character is a number 1-4 representing the eye number
  // Character 3-12 is a integer of the current "time" of the Polaris box when the signal is received from the eye
  // Character 13 is a carriage return. which is ASCII decimal (13) or hex "D"
  //
  // Setting this to 12 because the last byte is a CR that we are splitting on
  static constexpr int FARMTEK_MSG_EXPECTED_SIZE = 12;
  
  QByteArray mDataBuffer;
};

AUTOXTIME_NAMESPACE_END

#endif // AUTOXTIME_CODEC_FARMTEKCODEC
