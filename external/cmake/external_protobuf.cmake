ExternalProject_Add(external_protobuf
  PREFIX ${external_EXTERNAL_PREFIX}
  INSTALL_DIR ${external_INSTALL_PREFIX}
  GIT_REPOSITORY "https://github.com/protocolbuffers/protobuf.git"
  GIT_TAG        "v3.15.7"
  SOURCE_SUBDIR  cmake
  GIT_CONFIG advice.detachedHead=false
  LIST_SEPARATOR "|"
  CMAKE_ARGS ${external_DEFAULT_ARGS}
  -Dprotobuf_BUILD_TESTS:BOOL=OFF
  -Dprotobuf_BUILD_PROTOC_BINARIES:BOOL=ON
  -Dprotobuf_BUILD_CONFORMANCE:BOOL=OFF
  -Dprotobuf_BUILD_EXAMPLES:BOOL=OFF
  -DBUILD_SHARED_LIBS=ON
)

