##########################
# DB Listener

# create our executable from source
add_executable(autoxtime_db_listener "${CMAKE_CURRENT_LIST_DIR}/../db_listener.cpp")

target_compile_options(autoxtime_db_listener PUBLIC ${autoxtime_COMPILE_OPTIONS})

# Link against QtCore
target_link_libraries(autoxtime_db_listener autoxtime)
target_link_libraries(autoxtime_db_listener autoxtimeproto)
target_link_libraries(autoxtime_db_listener ${Protobuf_LIBRARIES})
target_link_libraries(autoxtime_db_listener Qt5::Core)
target_link_libraries(autoxtime_db_listener Qt5::Sql)
# not needed, can ignore timers executable soon anyways
target_link_libraries(autoxtime_db_listener Qt5::Widgets)

install(TARGETS autoxtime_db_listener
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib64
  ARCHIVE DESTINATION lib64
)
