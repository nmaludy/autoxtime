file(GLOB_RECURSE autoxtime_PROTO_FILES  "${CMAKE_CURRENT_LIST_DIR}/*.proto" )

# Find Protobuf
find_package(Protobuf REQUIRED)

# generate cpp/h files for all proto files
protobuf_generate_cpp(autoxtime_PROTO_SRCS autoxtime_PROTO_HDRS ${autoxtime_PROTO_FILES})

# set(autoxtime_PROTO_SRCS ${autoxtime_PROTO_SRCS} PARENT_SCOPE)
# set(autoxtime_PROTO_HDRS ${autoxtime_PROTO_HDRS} PARENT_SCOPE)
message("autoxtime_PROTO_SRCS = ${autoxtime_PROTO_SRCS}")

set(Protobuf_LIBRARIES ${Protobuf_LIBRARIES} PARENT_SCOPE)

add_library(autoxtimeproto SHARED ${autoxtime_PROTO_SRCS} ${autoxtime_PROTO_HDRS})

target_include_directories(autoxtimeproto PUBLIC "${autoxtime_INSTALL_PREFIX}/include")

target_link_libraries(autoxtimeproto ${Protobuf_LIBRARIES})

install(TARGETS autoxtimeproto
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib64
  ARCHIVE DESTINATION lib64
  )
