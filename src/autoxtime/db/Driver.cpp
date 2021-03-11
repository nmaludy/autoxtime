#include <autoxtime/db/Driver.h>

AUTOXTIME_NAMESPACE_BEG

std::vector<autoxtime::proto::Driver> Driver::Driver::list()
{
  std::vector<autoxtime::proto::Driver> drivers;
  
  autoxtime::proto::Driver d;
  d.set_first_name("Nick");
  drivers.push_back(d);
  
  autoxtime::proto::Driver d2;
  d2.set_first_name("brad");
  drivers.push_back(d2);

  return drivers;
}

AUTOXTIME_NAMESPACE_END
