##########################
# Timers

# create our executable from source
add_executable(autoxtime_timers "${CMAKE_CURRENT_LIST_DIR}/../timers.cpp")

target_compile_options(autoxtime_timers PUBLIC ${autoxtime_COMPILE_OPTIONS})

# Link against QtCore
target_link_libraries(autoxtime_timers autoxtime)
target_link_libraries(autoxtime_timers autoxtimeproto)
target_link_libraries(autoxtime_timers ${Protobuf_LIBRARIES})
target_link_libraries(autoxtime_timers Qt5::Core)
target_link_libraries(autoxtime_timers Qt5::SerialPort)
target_link_libraries(autoxtime_timers Qt5::Sql)
# not needed, can ignore timers executable soon anyways
target_link_libraries(autoxtime_timers Qt5::Widgets)

install(TARGETS autoxtime_timers
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib64
  ARCHIVE DESTINATION lib64
)
