##########################
# autoxtime (main window)

# create our executable from source
add_executable(autoxtime_ui "${CMAKE_CURRENT_LIST_DIR}/../autoxtime.cpp")

# Link against QtCore
target_link_libraries(autoxtime_ui autoxtime)
target_link_libraries(autoxtime_ui Qt5::Core)
target_link_libraries(autoxtime_ui Qt5::SerialPort)
target_link_libraries(autoxtime_ui Qt5::Widgets)

install(TARGETS autoxtime_ui
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib64
  ARCHIVE DESTINATION lib64
)
