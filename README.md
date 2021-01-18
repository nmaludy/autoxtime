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
```shell
tar -xvf qt-everywhere-src-5.15.2.tar.xz
cd qt-everywhere-src-5.15.2
./configure -developer-build -opengl -release -nomake tests -no-openssl -no-cups -no-qml-debug -opensource -confirm-license -no-sql-mysql -no-sql-psql -no-sql-sqlite -no-sql-odbc -skip webengine -prefix /usr/local/Qt-5.12.2
make -j`nproc`
sudo make install
```
