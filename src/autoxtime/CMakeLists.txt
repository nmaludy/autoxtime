# so we can find our CMake files
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake" ${CMAKE_MODULE_PATH})

# find all source files
file(GLOB_RECURSE PROJECT_SRC  "${CMAKE_CURRENT_LIST_DIR}/*.cpp" )
# filter out executables
list(FILTER PROJECT_SRC EXCLUDE REGEX "exe/.*")

# Find Qt
find_package(Qt5 COMPONENTS Core SerialPort Widgets REQUIRED)

# build our autoxtime library
include(autoxtime)

# build our executables
add_subdirectory(exe)
