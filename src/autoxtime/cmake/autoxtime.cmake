##########################
# autoxtime library
message("PROJECT_SRC = ${PROJECT_SRC}")

add_library(autoxtime SHARED ${PROJECT_SRC})

# add our directory for finding headers
# this must come after defining our executables
target_include_directories(autoxtime PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../..")
# rapidcsv is stupid and installs into lib/ instead of include/
target_include_directories(autoxtime PUBLIC "${autoxtime_INSTALL_PREFIX}/lib")

# Link against QtCore
target_link_libraries(autoxtime Qt5::Core)
target_link_libraries(autoxtime Qt5::SerialPort)
# not needed, can ignore timers execuoon anyways
target_link_libraries(autoxtime Qt5::Widgets)

install(TARGETS autoxtime
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib64
  ARCHIVE DESTINATION lib64
  )

