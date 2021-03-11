ExternalProject_Add(external_cutelyst
  PREFIX ${external_EXTERNAL_PREFIX}
  INSTALL_DIR ${external_INSTALL_PREFIX}
  GIT_REPOSITORY "https://github.com/cutelyst/cutelyst.git"
  GIT_TAG "v2.14.2"
  GIT_CONFIG advice.detachedHead=false
  LIST_SEPARATOR "|"
  CMAKE_ARGS ${external_DEFAULT_ARGS} -DPLUGIN_VIEW_GRANTLEE=on -DPLUGIN_VIEW_CLEARSILVER=off  -DPLUGIN_VIEW_EMAIL=off -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=on
)
ExternalProject_Add_StepDependencies(external_cutelyst build external_grantlee)
