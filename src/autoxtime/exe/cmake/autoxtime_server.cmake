##########################
# Server

# create our executable from source
add_executable(autoxtime_server "${CMAKE_CURRENT_LIST_DIR}/../server.cpp")

target_include_directories(autoxtime_server PUBLIC "${autoxtime_INSTALL_PREFIX}/include/cutelyst2-qt5")

# Link against QtCore
target_link_libraries(autoxtime_server autoxtime)
target_link_libraries(autoxtime_server autoxtimeserver)
target_link_libraries(autoxtime_server Qt5::Core)
target_link_libraries(autoxtime_server Qt5::Sql)
target_link_libraries(autoxtime_server Qt5::SerialPort)
# not needed, can ignore server executable soon anyways
target_link_libraries(autoxtime_server Qt5::Widgets)

install(TARGETS autoxtime_server
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib64
  ARCHIVE DESTINATION lib64
)
