# so we can find our CMake files
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake" ${CMAKE_MODULE_PATH})

# find all source files
file(GLOB_RECURSE PROJECT_SRC  "${CMAKE_CURRENT_LIST_DIR}/*.cpp" )
# filter out executables
list(FILTER PROJECT_SRC EXCLUDE REGEX "exe/.*")
list(FILTER PROJECT_SRC EXCLUDE REGEX "server/.*")
# one exception and that is AppCommon, should we move this to a different folder?
list(APPEND PROJECT_SRC "exe/AppCommon.cpp")

# Find Qt
find_package(Qt5 COMPONENTS Core SerialPort Sql Widgets REQUIRED)

# include our protobuf directory FIRST, so that we can append these source to PROJECT_SRC
add_subdirectory(proto)

# build our autoxtime library
include(autoxtime)

# build our server
# add_subdirectory(server)

# build our executables (requires our server executables)
add_subdirectory(exe)
