ExternalProject_Add(external_grantlee
  PREFIX ${autoxtime_EXTERNAL_PREFIX}
  INSTALL_DIR ${autoxtime_INSTALL_PREFIX}
  GIT_REPOSITORY "https://github.com/steveire/grantlee.git"
  GIT_TAG "v5.2.0"
  LIST_SEPARATOR "|"
  CMAKE_ARGS ${autoxtime_DEFAULT_ARGS} -DBUILD_TESTS=off
)
