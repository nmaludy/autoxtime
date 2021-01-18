cmake_minimum_required(VERSION 3.10...3.19)

# set the project name
project(autoxtime
  VERSION 1.0
  DESCRIPTION "AutoX Timing System"
  LANGUAGES CXX)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

# # setup static builds
# set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
# set(BUILD_SHARED_LIBS OFF)
# set(CMAKE_EXE_LINKER_FLAGS "-static")

# turn on debug builds
# set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)

# put all executables in the build/ directory root rather than in src/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# Qt things
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
# TODO how to make this more dynamic?
list(APPEND CMAKE_PREFIX_PATH "/usr/local/Qt-5.12.2")


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

# Add any targets in our source directory
add_subdirectory(src)

