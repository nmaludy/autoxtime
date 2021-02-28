# find all source files and include them in our executable
file(GLOB_RECURSE PROJECT_SRC  "autoxtime/*.cpp" )

message(STATUS "PROJECT_SRC = ${PROJECT_SRC}")

# filter out executables, so each has their own
list(FILTER PROJECT_SRC EXCLUDE REGEX "autoxtime/exe/.*")

# create a new variable to exclude UI components
set(PROJECT_SRC_NO_UI ${PROJECT_SRC})
list(FILTER PROJECT_SRC_NO_UI EXCLUDE REGEX "autoxtime/ui/.*")

# Find Qt
find_package(Qt5 COMPONENTS Core SerialPort Widgets REQUIRED)

# TODO, split these up into different cmake files, one per exe
# TODO, create a common library that these two link into to avoid duplicate compilation

##########################
# Timers

# create our executable from source
add_executable(${PROJECT_NAME}_timers
  ${PROJECT_SRC_NO_UI}
  autoxtime/exe/timers.cpp)

# add our directory for finding headers
# this must come after defining our executables
target_include_directories(${PROJECT_NAME}_timers PUBLIC ".")
target_include_directories(${PROJECT_NAME}_timers PUBLIC ${PROJECT_EXTERN_DIR}/lib)

# Link against QtCore
target_link_libraries(${PROJECT_NAME}_timers Qt5::Core)
target_link_libraries(${PROJECT_NAME}_timers Qt5::SerialPort)


##########################
# autoxtime (main window)

# create our executable from source
add_executable(${PROJECT_NAME}
  ${PROJECT_SRC}
  autoxtime/exe/autoxtime.cpp)

# add our directory for finding headers
# this must come after defining our executables
target_include_directories(${PROJECT_NAME} PUBLIC ".")
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_EXTERN_DIR}/lib)

# Link against QtCore
target_link_libraries(${PROJECT_NAME} Qt5::Core)
target_link_libraries(${PROJECT_NAME} Qt5::SerialPort)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
