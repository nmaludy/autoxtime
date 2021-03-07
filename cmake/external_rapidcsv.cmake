ExternalProject_Add(external_rapidcsv
  PREFIX ${autoxtime_EXTERNAL_PREFIX}
  INSTALL_DIR ${autoxtime_INSTALL_PREFIX}
  GIT_REPOSITORY "https://github.com/d99kris/rapidcsv.git"
  GIT_TAG "v8.46"
  LIST_SEPARATOR "|"
  CMAKE_ARGS ${autoxtime_DEFAULT_ARGS} -DBUILD_TESTS=OFF -DBUILD_SHARED=ON -DWITH_DOCS=OFF
)
