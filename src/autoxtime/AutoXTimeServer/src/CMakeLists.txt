file(GLOB_RECURSE AutoXTimeServer_SRCS *.cpp *.h)

set(AutoXTimeServer_SRCS
    ${AutoXTimeServer_SRCS}
    ${TEMPLATES_SRC}
)

# Create the application
add_library(AutoXTimeServer SHARED ${AutoXTimeServer_SRCS})

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

