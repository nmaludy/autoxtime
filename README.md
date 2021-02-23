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
sudo yum install libxcb libxcb-devel xcb-util xcb-util-devel mesa-libGL-devel libxkbcommon-devel libinput-devel libxkbcommon-x11-devel xorg-x11-xkb-utils-devel libxcb libxcb-devel xcb-util xcb-util-devel mesa-libGL-devel libxkbcommon-devel xcb-util-renderutil-devel xcb-proto xcb-util-cursor cb-util-cursor-devel compat-libxcb

./configure -developer-build -opengl -release -nomake tests -xcb -xcb-xlib -no-openssl -no-cups -no-qml-debug -opensource -confirm-license -no-sql-mysql -no-sql-psql -no-sql-sqlite -no-sql-odbc -skip webengine -prefix /usr/local/Qt-5.12.2
make -j`nproc`
sudo make install
```
