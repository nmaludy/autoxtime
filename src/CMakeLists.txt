# find all source files and include them in our executable
file(GLOB_RECURSE PROJECT_SRC  "autoxtime/*.cpp" )

list(APPEND INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS "autoxtime CMAKE_PREFIX_PATH = ${CMAKE_PREFIX_PATH}")
message(STATUS "autoxtime INCLUDE_DIRECTORIES = ${INCLUDE_DIRECTORIES}")

# filter out executables, so each has their own
list(FILTER PROJECT_SRC EXCLUDE REGEX "autoxtime/exe/.*")
list(FILTER PROJECT_SRC EXCLUDE REGEX "autoxtime/AutoXTimeServer/.*")

# create a new variable to exclude UI components
set(PROJECT_SRC_NO_UI ${PROJECT_SRC})
list(FILTER PROJECT_SRC_NO_UI EXCLUDE REGEX "autoxtime/ui/.*")

# Find Qt
find_package(Qt5 COMPONENTS Core SerialPort Widgets REQUIRED)

# TODO, split these up into different cmake files, one per exe
# TODO, create a common library that these two link into to avoid duplicate compilation

##########################
# autoxtime library
add_library(${PROJECT_NAME} SHARED ${PROJECT_SRC})

# add our directory for finding headers
# this must come after defining our executables
target_include_directories(${PROJECT_NAME} PUBLIC ".")
target_include_directories(${PROJECT_NAME} PUBLIC ${autoxtime_INSTALL_PREFIX}/lib)
target_include_directories(${PROJECT_NAME} PUBLIC ${autoxtime_INSTALL_PREFIX}/include)

# Link against QtCore
target_link_libraries(${PROJECT_NAME} Qt5::Core)
target_link_libraries(${PROJECT_NAME} Qt5::SerialPort)
# not needed, can ignore timers execuoon anyways
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
add_dependencies(${PROJECT_NAME} external_rapidcsv)


##########################
# Timers

# create our executable from source
add_executable(${PROJECT_NAME}_timers
  autoxtime/exe/timers.cpp)

# add our directory for finding headers
# this must come after defining our executables
target_include_directories(${PROJECT_NAME}_timers PUBLIC ".")
target_include_directories(${PROJECT_NAME} PUBLIC ${autoxtime_INSTALL_PREFIX}/include)

# Link against QtCore
target_link_libraries(${PROJECT_NAME}_timers ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}_timers Qt5::Core)
target_link_libraries(${PROJECT_NAME}_timers Qt5::SerialPort)
# not needed, can ignore timers executable soon anyways
target_link_libraries(${PROJECT_NAME}_timers Qt5::Widgets)


##########################
# autoxtime (main window)

# create our executable from source
add_executable(${PROJECT_NAME}_ui
  autoxtime/exe/autoxtime.cpp)

# add our directory for finding headers
# this must come after defining our executables
target_include_directories(${PROJECT_NAME}_ui PUBLIC ".")

# Link against QtCore
target_link_libraries(${PROJECT_NAME}_ui ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}_ui Qt5::Core)
target_link_libraries(${PROJECT_NAME}_ui Qt5::SerialPort)
target_link_libraries(${PROJECT_NAME}_ui Qt5::Widgets)


########################
# Server
add_subdirectory(autoxtime/AutoXTimeServer)

# ########################
# # Server
# include(ExternalProject)

# set(_source "${CMAKE_CURRENT_SOURCE_DIR}/autoxtime/AutoXTimeServer")
# set(_build "${CMAKE_CURRENT_BINARY_DIR}/autoxtime/AutoXTimeServer")
# set(_install "${CMAKE_CURRENT_BINARY_DIR}/prefix")

# message(STATUS "CMAKE_PREFIX_PATH= '${CMAKE_PREFIX_PATH}'")

# # https://stackoverflow.com/questions/45414507/pass-a-list-of-prefix-paths-to-externalproject-add-in-cmake-args
# string(REPLACE ";" "|" CMAKE_PREFIX_PATH_ALT_SEP "${CMAKE_PREFIX_PATH}")

# ExternalProject_Add(AutoXTimeServer
#   SOURCE_DIR ${_source}
#   BINARY_DIR ${_build}
#   INSTALL_DIR ${_install}
#   INSTALL_COMMAND ""
#   LIST_SEPARATOR | # Use the alternate list separator
#   CMAKE_ARGS -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH_ALT_SEP} -DINCLUDE_DIRECTORIES=${INCLUDE_DIRECTORIES} -DLINK_DIRECTORIES=${LINK_DIRECTORIES}
#   DEPENDS extern_grantlee extern_grantlee
# )
