
message(STATUS "autoxtimeserver src CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")

file(GLOB_RECURSE AutoXTimeServer_SRCS *.cpp *.h)

set(AutoXTimeServer_SRCS
    ${AutoXTimeServer_SRCS}
    ${TEMPLATES_SRC}
)

# Create the application
add_library(AutoXTimeServer SHARED ${AutoXTimeServer_SRCS})

add_dependencies(AutoXTimeServer extern_cutelyst)
find_package(Qt5 COMPONENTS Core Network Sql REQUIRED)
find_package(Cutelyst2Qt5 REQUIRED)

# Link to Cutelyst
target_link_libraries(AutoXTimeServer
    Cutelyst::Core
    Cutelyst::StaticSimple
    Cutelyst::View::Grantlee
    Cutelyst::Utils::Sql
    Cutelyst::Session
    Cutelyst::Authentication
    Qt5::Core
    Qt5::Network
    Qt5::Sql
)




