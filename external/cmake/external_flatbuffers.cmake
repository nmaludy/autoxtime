ExternalProject_Add(external_flatbuffers
  PREFIX ${external_EXTERNAL_PREFIX}
  INSTALL_DIR ${external_INSTALL_PREFIX}
  GIT_REPOSITORY "https://github.com/google/flatbuffers.git"
  GIT_TAG "v1.12.0"
  GIT_CONFIG advice.detachedHead=false
  LIST_SEPARATOR "|"
  CMAKE_ARGS ${external_DEFAULT_ARGS}
)
