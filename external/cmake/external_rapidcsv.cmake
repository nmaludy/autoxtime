message("external_EXTERNAL_PREFIX = ${external_EXTERNAL_PREFIX}")

ExternalProject_Add(external_rapidcsv
  PREFIX ${external_EXTERNAL_PREFIX}
  INSTALL_DIR ${external_INSTALL_PREFIX}
  GIT_REPOSITORY "https://github.com/d99kris/rapidcsv.git"
  GIT_TAG "v8.46"
  GIT_CONFIG advice.detachedHead=false
  LIST_SEPARATOR "|"
  CMAKE_ARGS ${external_DEFAULT_ARGS} -DBUILD_TESTS=OFF -DBUILD_SHARED=ON -DWITH_DOCS=OFF
)
