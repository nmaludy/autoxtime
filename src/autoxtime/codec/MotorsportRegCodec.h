#ifndef AUTOXTIME_TRANSPORT_MOTORSPORTSREGREADER
#define AUTOXTIME_TRANSPORT_MOTORSPORTSREGREADER

#include <autoxtime/autoxtime.h>
#include <autoxtime/proto/driver.pb.h>
#include <autoxtime/proto/car.pb.h>
#include <autoxtime/proto/car_class.pb.h>
#include <autoxtime/proto/event_registration.pb.h>

#include <memory>

#include <QObject>

namespace google::protobuf { class FieldDescriptor; }

AUTOXTIME_NAMESPACE_BEG

class MotorsportRegEntry
{
 public:
  MotorsportRegEntry();

  std::unique_ptr<autoxtime::proto::Driver> mpDriver;
  std::unique_ptr<autoxtime::proto::Car> mpCar;
  std::unique_ptr<autoxtime::proto::CarClass> mpCarClass;
  std::unique_ptr<autoxtime::proto::EventRegistration> mpEventRegistration;
};

class MotorsportRegCodec : public QObject
{
  Q_OBJECT

 public:
  explicit MotorsportRegCodec(QObject* pParent = nullptr);

  std::vector<std::shared_ptr<MotorsportRegEntry>> readFile(const QString& fileName);

 private:
  enum Proto
  {
    MOTORSPORTSREG_PROTO_DRIVER,
    MOTORSPORTSREG_PROTO_CAR,
    MOTORSPORTSREG_PROTO_CAR_CLASS,
    MOTORSPORTSREG_PROTO_EVENT_REGISTRATION,
  };

  class Mapping
  {
   public:
    Mapping(MotorsportRegCodec::Proto proto,
            const std::string& fieldName);

    MotorsportRegCodec::Proto mProto;
    const google::protobuf::FieldDescriptor* mpField;
  };

  static const std::unordered_map<std::string, Mapping> COLUMN_MAP;
};

AUTOXTIME_NAMESPACE_END

#endif // AUTOXTIME_TRANSPORT_SERIALPORTASYNCREADER
