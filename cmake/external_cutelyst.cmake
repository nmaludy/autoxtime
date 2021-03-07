ExternalProject_Add(external_cutelyst
  PREFIX ${autoxtime_EXTERNAL_PREFIX}
  INSTALL_DIR ${autoxtime_INSTALL_PREFIX}
  GIT_REPOSITORY "https://github.com/cutelyst/cutelyst.git"
  GIT_TAG "v2.14.2"
  LIST_SEPARATOR "|"
  CMAKE_ARGS ${autoxtime_DEFAULT_ARGS} -DPLUGIN_VIEW_GRANTLEE=on -DPLUGIN_VIEW_CLEARSILVER=off  -DPLUGIN_VIEW_EMAIL=off
)
ExternalProject_Add_StepDependencies(external_cutelyst build external_grantlee)
