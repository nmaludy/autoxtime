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

###########
# Testing
if(${TEST_NAME} EQUAL "clang-tidy")
  set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-header-filter=include;-checks=*;-warnings-as-errors=*;")
elseif(${TEST_NAME} EQUAL "cppcheck")
  set(CMAKE_CXX_CPPCHECK "cppcheck")
elseif(${TEST_NAME} EQUAL "include-what-you-use")
  set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "iwyu")
elseif(${TEST_NAME} EQUAL "link-what-you-use")
  set(CMAKE_LINK_WHAT_YOU_USE TRUE)
endif()
  
  
# find all source files and include them in our executable
file(GLOB PROJECT_SRC "src/*.cpp" )

# create our executable from source
add_executable(${PROJECT_NAME} ${PROJECT_SRC})

# add our include directory for finding headers
# this must come after defining our executable 
target_include_directories(${PROJECT_NAME} PUBLIC "include")

