##########################
# autoxtime library
message("PROJECT_SRC = ${PROJECT_SRC}")

add_library(autoxtime SHARED ${PROJECT_SRC})

target_compile_options(autoxtime PUBLIC ${autoxtime_COMPILE_OPTIONS})

# add our directory for finding headers
# this must come after defining our executables
target_include_directories(autoxtime PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../..")
# protobuf generates its files in build/src/autoxtime/proto
target_include_directories(autoxtime PUBLIC "${autoxtime_BINARY_DIR}/src")

# Link against QtCore
target_link_libraries(autoxtime autoxtimeproto)
target_link_libraries(autoxtime ${Protobuf_LIBRARIES})
target_link_libraries(autoxtime Qt5::Core)
target_link_libraries(autoxtime Qt5::SerialPort)
target_link_libraries(autoxtime Qt5::Sql)
# not needed, can ignore timers execuoon anyways
target_link_libraries(autoxtime Qt5::Widgets)

install(TARGETS autoxtime
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib64
  ARCHIVE DESTINATION lib64
  )

