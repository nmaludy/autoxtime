ExternalProject_Add(external_grantlee
  PREFIX ${external_EXTERNAL_PREFIX}
  INSTALL_DIR ${external_INSTALL_PREFIX}
  GIT_REPOSITORY "https://github.com/steveire/grantlee.git"
  GIT_TAG "v5.2.0"
  GIT_CONFIG advice.detachedHead=false
  LIST_SEPARATOR "|"
  CMAKE_ARGS ${external_DEFAULT_ARGS} -DBUILD_TESTS=off
)
