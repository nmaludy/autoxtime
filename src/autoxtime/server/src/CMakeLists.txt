
message(STATUS "autoxtimeserver src CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")

file(GLOB_RECURSE autoxtimeserver_SRCS *.cpp *.h)

set(autoxtimeserver_SRCS
    ${autoxtimeserver_SRCS}
    ${TEMPLATES_SRC}
)

# Create the application
add_library(autoxtimeserver SHARED ${autoxtimeserver_SRCS})
add_dependencies(autoxtimeserver external_grantlee)
add_dependencies(autoxtimeserver external_cutelyst)

find_package(Qt5 COMPONENTS Core Network Sql REQUIRED)
# can't use find_package for cutelyst because we're installing that with externalproject_add
# this function works at build time (make) rather than at cmake time where find_package runs
# find_package(Cutelyst2Qt5)

target_include_directories(autoxtimeserver PUBLIC "${autoxtime_INSTALL_PREFIX}/include/cutelyst2-qt5")

target_link_libraries(autoxtimeserver Qt5::Core)
target_link_libraries(autoxtimeserver Qt5::Network)
target_link_libraries(autoxtimeserver Qt5::Sql)

# Link to Cutelyst
#
# TODO prune these down to only the ones we need
target_link_libraries(autoxtimeserver
  Cutelyst2Qt5
  Cutelyst2Qt5Authentication
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

if (NOT WIN32)
  target_link_libraries(autoxtimeserver 
    Cutelyst2Qt5EventLoopEpoll)
endif()

install(TARGETS autoxtimeserver
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib64
  ARCHIVE DESTINATION lib64
)
