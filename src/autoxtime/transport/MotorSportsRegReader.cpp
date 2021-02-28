#include <autoxtime/transport/MotorSportsRegReader.h>

// autoxtime
#include <autoxtime/config/ConfigStore.h>

// etc
#include <rapidcsv.h>

// std
#include <iostream>
#include <string>
#include <vector>

AUTOXTIME_NAMESPACE_BEG

MotorSportsRegReader::MotorSportsRegReader(QObject* pParent)
    : ITransport(pParent)
{}

void MotorSportsRegReader::readFile(const QString& fileName)
{
  rapidcsv::Document doc("../data/20201004-AxWareEventExport.csv");
  std::cout << "Read " << doc.GetRowCount() << " values." << std::endl;
  std::vector<std::string> column_names = doc.GetColumnNames();
  std::cout << "column names:" << std::endl;
  for (const std::string& column : column_names) {
    std::cout << "  - " << column << std::endl;
  }

  std::cout << "Read " << doc.GetRowCount() << " values." << std::endl;
  const std::size_t row_count = doc.GetRowCount();
  const std::size_t col_idx = doc.GetColumnIdx("First Name");
  for (std::size_t row_idx = 0; row_idx < row_count; ++row_idx) {
    std::cout << "First name = " << doc.GetCell<std::string>(col_idx, row_idx) << std::endl;
  }
}

AUTOXTIME_NAMESPACE_END
