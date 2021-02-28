#
# Copyright (c) 2020 KeySpoke Software
#

# cmake_* commands must be in the parent file
cmake_minimum_required(VERSION 3.14.7)

# this makes link directories relative to source dir
cmake_policy(SET CMP0015 NEW)

# Honor _ROOT variables
cmake_policy(SET CMP0074 NEW)

# Required
project(autoxtime_externs)
set(PROJECT_EXTERN_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

include(ExternalProject)

###################
# externs
ExternalProject_Add(extern_rapidcsv
  PREFIX ${PROJECT_EXTERN_DIR}
  GIT_REPOSITORY "https://github.com/d99kris/rapidcsv.git"
  GIT_TAG "v8.46"
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PROJECT_EXTERN_DIR} -DBUILD_TESTS=OFF -DBUILD_SHARED=ON -DWITH_DOCS=OFF
)

ExternalProject_Add(extern_grantlee
  PREFIX ${PROJECT_EXTERN_DIR}
  GIT_REPOSITORY "https://github.com/steveire/grantlee.git"
  GIT_TAG "v5.2.0"
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PROJECT_EXTERN_DIR} -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH} -DBUILD_TESTS=off
)

ExternalProject_Add(extern_cutelyst
  PREFIX ${PROJECT_EXTERN_DIR}
  GIT_REPOSITORY "https://github.com/cutelyst/cutelyst.git"
  GIT_TAG "v2.14.2"
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PROJECT_EXTERN_DIR} -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH} -DPLUGIN_VIEW_GRANTLEE=on -DPLUGIN_VIEW_CLEARSILVER=off  -DPLUGIN_VIEW_EMAIL=off
  )
ExternalProject_Add_StepDependencies(extern_cutelyst build extern_grantlee)

# # ASIO is header-only, so there's no build command here
# # ASIO uses autotools for configuring/building
# ExternalProject_Add(extern_asio
#   PREFIX ${STAG_EXTERN_DIR}
#   URL "https://github.com/ben-strasser/fast-cpp-csv-parser"
#   CONFIGURE_COMMAND ${STAG_EXTERN_DIR}/src/extern_asio/configure --prefix=${STAG_EXTERN_DIR}
#   BUILD_IN_SOURCE ON
#   BUILD_COMMAND ""
#   INSTALL_COMMAND make -C ${STAG_EXTERN_DIR}/src/extern_asio/include install
# )
