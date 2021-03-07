
message(STATUS "autoxtimeserver src CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")

file(GLOB_RECURSE AutoXTimeServer_SRCS *.cpp *.h)

set(AutoXTimeServer_SRCS
    ${AutoXTimeServer_SRCS}
    ${TEMPLATES_SRC}
)

# Create the application
add_library(AutoXTimeServer SHARED ${AutoXTimeServer_SRCS})
add_dependencies(AutoXTimeServer external_grantlee)
add_dependencies(AutoXTimeServer external_cutelyst)

find_package(Qt5 COMPONENTS Core Network Sql REQUIRED)
# can't use find_package for cutelyst because we're installing that with externalproject_add
# this function works at build time (make) rather than at cmake time where find_package runs
# find_package(Cutelyst2Qt5)

# rapidcsv is stupid and installs into lib/ instead of include/
target_include_directories(AutoXTimeServer PUBLIC "${autoxtime_INSTALL_PREFIX}/include/cutelyst2-qt5")

target_link_libraries(AutoXTimeServer Qt5::Core)
target_link_libraries(AutoXTimeServer Qt5::Network)
target_link_libraries(AutoXTimeServer Qt5::Sql)

# Link to Cutelyst
#
# TODO prune these down to only the ones we need
target_link_libraries(AutoXTimeServer
  Cutelyst2Qt5
  Cutelyst2Qt5Authentication
  Cutelyst2Qt5EventLoopEpoll
  Cutelyst2Qt5Session
  Cutelyst2Qt5StaticSimple
  Cutelyst2Qt5StatusMessage
  Cutelyst2Qt5UserAgent
  Cutelyst2Qt5UtilsLangSelect
  Cutelyst2Qt5UtilsPagination
  Cutelyst2Qt5UtilsSql
  Cutelyst2Qt5UtilsValidator
  Cutelyst2Qt5ViewGrantlee
  Cutelyst2Qt5ViewJson
  Cutelyst2Qt5Wsgi)

install(TARGETS AutoXTimeServer
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib64
  ARCHIVE DESTINATION lib64
)
