# find all source files and include them in our executable
file(GLOB_RECURSE PROJECT_SRC  "autoxtime/*.cpp" )

message(STATUS "src CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")
message(STATUS "PROJECT_SRC = ${PROJECT_SRC}")

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
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_EXTERN_DIR}/lib)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_EXTERN_DIR}/include)

# Link against QtCore
target_link_libraries(${PROJECT_NAME} Qt5::Core)
target_link_libraries(${PROJECT_NAME} Qt5::SerialPort)
# not needed, can ignore timers execuoon anyways
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)


##########################
# Timers

# create our executable from source
add_executable(${PROJECT_NAME}_timers
  autoxtime/exe/timers.cpp)

# add our directory for finding headers
# this must come after defining our executables
target_include_directories(${PROJECT_NAME}_timers PUBLIC ".")
target_include_directories(${PROJECT_NAME}_timers PUBLIC ${PROJECT_EXTERN_DIR}/lib)
target_include_directories(${PROJECT_NAME}_timers PUBLIC ${PROJECT_EXTERN_DIR}/include)

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
target_include_directories(${PROJECT_NAME}_ui PUBLIC ${PROJECT_EXTERN_DIR}/lib)
target_include_directories(${PROJECT_NAME}_timers PUBLIC ${PROJECT_EXTERN_DIR}/include)

# Link against QtCore
target_link_libraries(${PROJECT_NAME}_ui ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}_ui Qt5::Core)
target_link_libraries(${PROJECT_NAME}_ui Qt5::SerialPort)
target_link_libraries(${PROJECT_NAME}_ui Qt5::Widgets)


########################
# Server
add_subdirectory(autoxtime/AutoXTimeServer)
