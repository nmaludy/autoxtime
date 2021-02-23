# find all source files and include them in our executable
file(GLOB PROJECT_SRC "autoxtime/*.cpp" )

# Find Qt
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS SerialPort REQUIRED)

# create our executable from source
add_executable(${PROJECT_NAME}_timers
  autoxtime/exe/timers.cpp
  autoxtime/codec/FarmtekCodec.cpp
  autoxtime/config/ConfigStore.cpp
  autoxtime/transport/SerialPortAsyncReader.cpp
  autoxtime/transport/ITransport.cpp)

# add our directory for finding headers
# this must come after defining our executables
target_include_directories(${PROJECT_NAME}_timers PUBLIC ".")

# Link against QtCore
target_link_libraries(${PROJECT_NAME}_timers Qt5::Core)
target_link_libraries(${PROJECT_NAME}_timers Qt5::SerialPort)
