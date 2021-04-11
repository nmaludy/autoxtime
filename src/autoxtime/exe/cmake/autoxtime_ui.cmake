##########################
# autoxtime (main window)

# create our executable from source
add_executable(autoxtime_ui "${CMAKE_CURRENT_LIST_DIR}/../autoxtime.cpp")

target_compile_options(autoxtime_ui PUBLIC ${autoxtime_COMPILE_OPTIONS})

# Link against QtCore
target_link_libraries(autoxtime_ui autoxtime)
target_link_libraries(autoxtime_ui autoxtimeproto)
target_link_libraries(autoxtime_ui ${Protobuf_LIBRARIES})
target_link_libraries(autoxtime_ui Qt5::Core)
target_link_libraries(autoxtime_ui Qt5::SerialPort)
target_link_libraries(autoxtime_ui Qt5::Widgets)
target_link_libraries(autoxtime_ui Qt5::Sql)

install(TARGETS autoxtime_ui
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib64
  ARCHIVE DESTINATION lib64
)
