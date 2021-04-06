#include <autoxtime/codec/MotorsportRegCodec.h>

// autoxtime
#include <autoxtime/config/ConfigStore.h>
#include <autoxtime/db/BaseModel.h>

// etc
#include <rapidcsv.h>

// std
#include <iostream>
#include <string>
#include <vector>

// Qt
#include <QFile>

AUTOXTIME_NAMESPACE_BEG

const std::unordered_map<std::string, MotorsportRegCodec::Mapping> MotorsportRegCodec::COLUMN_MAP = {
  // TODO split this by following
  // format: class-subclass
  { "Class", { MOTORSPORTSREG_PROTO_CAR_CLASS, "name"} },
  { "Number", { MOTORSPORTSREG_PROTO_CAR, "car_number" } },
  { "First Name", { MOTORSPORTSREG_PROTO_DRIVER, "first_name" } },
  { "Last Name", { MOTORSPORTSREG_PROTO_DRIVER, "last_name" } },
  { "Car Model", { MOTORSPORTSREG_PROTO_CAR, "model" } },
  { "Car Color", { MOTORSPORTSREG_PROTO_CAR, "color" } },
  // "Registered",  (yes/no)
  // "Paid",  (yes/no)
  {"Amnt.", { MOTORSPORTSREG_PROTO_EVENT_REGISTRATION, "amount_paid" } },
  // "Group",
  {"Sponsor", { MOTORSPORTSREG_PROTO_CAR, "sponsor" } },
  {"Tire Brand", { MOTORSPORTSREG_PROTO_CAR, "tire_brand" } },
  // "Method",
  // "OLR",
  // "Member", Event Registration -> member yes/no
  {"Member #", { MOTORSPORTSREG_PROTO_DRIVER, "scca_id" } },
  // "Expires",
  {"Email #1", { MOTORSPORTSREG_PROTO_DRIVER, "email" } },
  // "Home",
  // "Work",
  {"Cell", { MOTORSPORTSREG_PROTO_DRIVER, "phone_number" } }
};

////////////////////////////////////////////

MotorsportRegEntry::MotorsportRegEntry()
    : mpDriver(std::make_unique<autoxtime::proto::Driver>()),
      mpCar(std::make_unique<autoxtime::proto::Car>()),
      mpCarClass(std::make_unique<autoxtime::proto::CarClass>()),
      mpEventRegistration(std::make_unique<autoxtime::proto::EventRegistration>())
{}

////////////////////////////////////////////

MotorsportRegCodec::Mapping::Mapping(MotorsportRegCodec::Proto proto,
                                      const std::string& fieldName)
    : mProto{proto},
      mpField{nullptr}
{
  const google::protobuf::Descriptor* p_desc = nullptr;
  switch (mProto)
  {
    case MOTORSPORTSREG_PROTO_DRIVER:
      p_desc = autoxtime::proto::Driver::descriptor();
      break;
    case MOTORSPORTSREG_PROTO_CAR:
      p_desc = autoxtime::proto::Car::descriptor();
      break;
    case MOTORSPORTSREG_PROTO_CAR_CLASS:
      p_desc = autoxtime::proto::CarClass::descriptor();
      break;
    case MOTORSPORTSREG_PROTO_EVENT_REGISTRATION:
      p_desc = autoxtime::proto::EventRegistration::descriptor();
      break;
  }
  mpField = p_desc->FindFieldByName(fieldName);
}

////////////////////////////////////////////

MotorsportRegCodec::MotorsportRegCodec(QObject* pParent)
    : QObject(pParent)
{}

std::vector<std::shared_ptr<MotorsportRegEntry>> MotorsportRegCodec
::readFile(const QString& fileName,
           std::shared_ptr<std::vector<QString>> pErrors)
{
  // read in the file, just for debugging, yes i'm anal
  QFile f(fileName);
  if (f.open(QFile::ReadOnly | QFile::Text))
  {
    QTextStream in(&f);
    AXT_DEBUG << "Parsing MotorsportsReg CSV file:\n"
              << in.readAll();
  }

  std::vector<std::shared_ptr<MotorsportRegEntry>> results;

  // read in CSV data
  rapidcsv::Document doc(fileName.toStdString());
  AXT_DEBUG << "Read rows: " << doc.GetRowCount();
  std::vector<std::string> column_names = doc.GetColumnNames();
  AXT_DEBUG << "Column names: ";
  for (const std::string& column : column_names)
  {
    AXT_DEBUG << " - " << column;
  }

  const std::size_t row_count = doc.GetRowCount();
  const std::size_t col_count = column_names.size();
  for (std::size_t row_idx = 0; row_idx < row_count; ++row_idx)
  {
    std::vector<std::string> row = doc.GetRow<std::string>(row_idx);
    std::shared_ptr<MotorsportRegEntry> entry = std::make_shared<MotorsportRegEntry>();
    for (std::size_t col_idx = 0; col_idx < col_count; ++col_idx)
    {
      const std::string& col_name = column_names[col_idx];
      // TODO setup a mapping based on column indexes for this file, rather than finding
      //      the mapping every time
      std::unordered_map<std::string, MotorsportRegCodec::Mapping>::const_iterator iter =
          COLUMN_MAP.find(col_name);
      if (iter == COLUMN_MAP.end())
      {
        continue;
      }

      const std::string& value = row[col_idx];
      const Mapping& mapping = iter->second;
      bool b_parsed = false;
      google::protobuf::Message* p_msg = nullptr;
      switch (mapping.mProto)
      {
        case MOTORSPORTSREG_PROTO_DRIVER:
          p_msg = entry->mpDriver.get();
          break;
        case MOTORSPORTSREG_PROTO_CAR:
          p_msg = entry->mpCar.get();
          break;
        case MOTORSPORTSREG_PROTO_CAR_CLASS:
          if (col_name == "Class")
          {
            std::unique_ptr<autoxtime::proto::CarClass>& p_car_class = entry->mpCarClass;
            // TODO split this by following
            // format: class-subclass
            //{ "Class", { MOTORSPORTSREG_PROTO_CAR_CLASS, "name"} },
            const std::string delimiter("-");
            std::size_t pos = value.find(delimiter);
            if (pos != std::string::npos)
            {
              std::string name = value.substr(0, pos);
              std::string subclass_name = value.substr(pos+delimiter.length(),
                                                       std::string::npos);
              p_car_class->set_name(name);
              p_car_class->set_subclass_name(subclass_name);
            }
            else
            {
              p_car_class->set_name(value);
            }
            // don't parse below, we parsed here
            b_parsed = true;
          }
          else
          {
            p_msg = entry->mpCarClass.get();
          }
          break;
        case MOTORSPORTSREG_PROTO_EVENT_REGISTRATION:
          p_msg = entry->mpEventRegistration.get();
          break;
      }

      if (p_msg && !b_parsed && !value.empty())
      {
        bool b_ok = autoxtime::db::BaseModel::setFieldVariant(p_msg,
                                                              mapping.mpField,
                                                              QVariant(QString::fromStdString(value)));
        if (!b_ok)
        {
          // TODO return errors back to user
          if (pErrors)
          {
            pErrors->push_back("Bad row");
          }
        }
      }
    }

    results.push_back(entry);
  }
  return results;
}

AUTOXTIME_NAMESPACE_END
