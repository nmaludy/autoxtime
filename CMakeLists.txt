cmake_minimum_required(VERSION 3.10...3.19)

# if windows, use MSYS/MinGW... not visualstudio (PITA)
if (WIN32)
  set(CMAKE_GENERATOR "MSYS Makefiles")
endif()

# set the project name
project(autoxtime
  VERSION 1.0
  DESCRIPTION "AutoX Timing System"
  LANGUAGES CXX)
set(autoxtime_INSTALL_PREFIX "${autoxtime_BINARY_DIR}/install-prefix")
set(autoxtime_AUTOXTIME_PREFIX "${autoxtime_BINARY_DIR}/autoxtime-prefix")
set(autoxtime_EXTERNAL_PREFIX "${autoxtime_BINARY_DIR}/external-prefix")
list(APPEND CMAKE_PREFIX_PATH ${autoxtime_INSTALL_PREFIX})

link_directories("${autoxtime_INSTALL_PREFIX}/lib")
link_directories("${autoxtime_INSTALL_PREFIX}/lib64")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)


# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Windows bullshit
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# setup shared lib builds
option(BUILD_SHARED_LIBS "Build libraries as SHARED" ON)

# # setup static builds
# set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
# set(BUILD_SHARED_LIBS OFF)
# set(CMAKE_EXE_LINKER_FLAGS "-static")

# turn on debug builds
# set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)

# output our executables in the build/ directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# Qt things
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
# find the Qt 5 installation and include it
# need to set this BEFORE including externs
if (WIN32)
  # hard coded here for github actions... annoying
  set(QT_DIR_LIST "D:/a/_temp/msys/msys64/mingw64/lib/")
else ()
  file(GLOB QT_DIR_LIST "/usr/local/Qt-5.*" )
endif()
message(STATUS "QT_DIR_LIST= '${QT_DIR_LIST}'")

# append QT directory that we found to CMAKE_PREFIX_PATH so that our sub-modules and external
# projects get it passed in on the CMake arguments (below)
list(APPEND CMAKE_PREFIX_PATH ${QT_DIR_LIST})

# Setup directorie so sub-projects work
# https://stackoverflow.com/questions/45414507/pass-a-list-of-prefix-paths-to-externalproject-add-in-cmake-args
string(REPLACE ";" "|" CMAKE_PREFIX_PATH_ALT_SEP "${CMAKE_PREFIX_PATH}")
set(autoxtime_DEFAULT_ARGS
  -G ${CMAKE_GENERATOR}
  -DCMAKE_VERBOSE_MAKEFILE=ON
  "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
  "-DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}"
  "-DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH_ALT_SEP}"
  "-DCMAKE_INSTALL_PREFIX:PATH=${autoxtime_INSTALL_PREFIX}")



###########
# Testing
if(${TEST_NAME})
  if(${TEST_NAME} EQUAL "clang-tidy")
    set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-header-filter=src;-checks=*;-warnings-as-errors=*;")
  elseif(${TEST_NAME} EQUAL "cppcheck")
    set(CMAKE_CXX_CPPCHECK "cppcheck")
  elseif(${TEST_NAME} EQUAL "include-what-you-use")
    set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "iwyu")
  elseif(${TEST_NAME} EQUAL "link-what-you-use")
    set(CMAKE_LINK_WHAT_YOU_USE cd ..TRUE)
  endif()
endif()

# allows us to include cmake files in cmake/ directory
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# Include some CMake modules that allow us to be really sweet
#
# allows us to download git repos at cmake time so we can consume them
# via find_package()
include(FetchContent)
# allows us to build external repos (cmake or otherwise)
include(ExternalProject)

# our includes
include(external_rapidcsv)
include(external_grantlee)
include(external_cutelyst)

# TODO convert our directory into an external project
add_subdirectory(src)
