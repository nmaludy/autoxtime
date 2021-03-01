# autoxtime
Autocross Timing System


### Development

Install CMake instructions [here](https://cmake.org/install/)

Build:
```shell
# create build/ directory with our Makefile
cmake -S . -B build

# compile/build the code
cmake --build build
```

#### Qt

Installing from source
https://wiki.qt.io/Building_Qt_5_from_Git
https://doc.qt.io/qt-5/linux-requirements.html
```shell
tar -xvf qt-everywhere-src-5.15.2.tar.xz
cd qt-everywhere-src-5.15.2
# https://wiki.qt.io/Building_Qt_5_from_Git
sudo yum install libxcb libxcb-devel xcb-util xcb-util-devel mesa-libGL-devel libxkbcommon-devel libinput-devel libxkbcommon-x11-devel xorg-x11-xkb-utils-devel libxcb libxcb-devel xcb-util xcb-util-devel mesa-libGL-devel libxkbcommon-devel xcb-util-renderutil-devel xcb-proto xcb-util-cursor cb-util-cursor-devel compat-libxcb postgresql-devel

./configure -developer-build -opengl -release -nomake tests -xcb -xcb-xlib -no-openssl -no-cups -no-qml-debug -opensource -confirm-license -sql-psql -no-sql-mysql -no-sql-sqlite -no-sql-odbc -skip webengine -prefix /usr/local/Qt-5.12.2
make -j`nproc`
sudo make install
```


#cutelsyst notes:

```shell
# Compiling c++
cmake .. -G "MinGW Makefiles" "-DCMAKE_PREFIX_PATH=C:\Users\bradl\Qt\5.15.2\mingw81_64\bin"
cmake --build .
cmake --build . --target install

# Programs
https://github.com/cutelyst/asql
https://github.com/cutelyst/cutelyst
https://github.com/cutelyst/cutelyst/wiki/Installing-Cutelyst
https://github.com/cutelyst/simple-mail
https://github.com/steveire/grantlee
https://github.com/cutelyst/cutelee

# Tutorial
https://github.com/cutelyst/cutelyst-tutorial/tree/master/Hello
https://github.com/cutelyst/cutelyst/wiki/Tutorial_02_CutelystBasics
https://doc.qt.io/qtvstools/qtvstools-getting-started.html

# Cutelyst
## compile
cmake .. -G "MinGW Makefiles" "-DCMAKE_PREFIX_PATH=C:\Users\bradl\Qt\5.15.2\mingw81_64\bin" -DPLUGIN_VIEW_GRANTLEE=on
cmake --build . --config Release
cmake --build . --target install --config Release

## New app
cutelyst2 --create-app AutoXTimeServer
### First time compile
cmake .. -G "MinGW Makefiles" "-DCMAKE_PREFIX_PATH=C:\Users\bradl\Qt\5.15.2\mingw81_64\bin"

## Add new controller
cutelyst2 --controller Login

## start server
cutelyst2 -r --server --app-file src/libAutoXTimeServer -- --chdir ..

## compile application (just updating files)
make

## If we add new components like a new controller, etc.
## We need to rebuild then compile
cmake ..
make
```
