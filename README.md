# autoxtime
Autocross Timing System


### Development

Install CMake instructions [here](https://cmake.org/install/)

Install+Start Postgres (CentOS 8) [reference](https://www.postgresql.org/download/linux/redhat/)
```shell
# Install the repository RPM:
sudo dnf install -y https://download.postgresql.org/pub/repos/yum/reporpms/EL-8-x86_64/pgdg-redhat-repo-latest.noarch.rpm

# Disable the built-in PostgreSQL module:
sudo dnf -qy module disable postgresql

# Install PostgreSQL:
sudo dnf install -y postgresql13-server

# Optionally initialize the database and enable automatic start:
sudo /usr/pgsql-13/bin/postgresql-13-setup initdb
sudo systemctl enable postgresql-13
sudo systemctl start postgresql-13

# create our database and load our schema
sudo -s -u postgres createdb autoxtime
cat db/db_table_create.sql | sudo -i -u postgres psql autoxtime -f -
cat db/db_user_create.sql | sudo -i -u postgres psql autoxtime -f -
```

Build:
```shell
# build our external dependencies in build/external using external/CMakeLists.txt 
cmake -S external -B build/external
cmake --build build/external

# build the autoxtime code directly in the build/ directory using ./CMakeLists
cmake -S . -B build
cmake --build build

# I've also added a helper script that does the above all in one command:
./bin/build.sh
```

Run:
```shell
# ui
./bin/autoxtime.sh

# standalone server
./bin/server.sh
```

#### Protobuf files

Protobuf spec files live in `src/autoxtime/proto`

They are generated during build time and are placed in `build/src/autoxtime/proto` in case
you need to look at the headers or cpp files.

#### Qt

Installing from source
https://wiki.qt.io/Building_Qt_5_from_Git
https://doc.qt.io/qt-5/linux-requirements.html
```shell
tar -xvf qt-everywhere-src-5.15.2.tar.xz
cd qt-everywhere-src-5.15.2
# https://wiki.qt.io/Building_Qt_5_from_Git
sudo yum install libxcb libxcb-devel xcb-util xcb-util-devel mesa-libGL-devel libxkbcommon-devel libinput-devel libxkbcommon-x11-devel xorg-x11-xkb-utils-devel libxcb libxcb-devel xcb-util xcb-util-devel mesa-libGL-devel libxkbcommon-devel xcb-util-renderutil-devel xcb-proto xcb-util-cursor cb-util-cursor-devel compat-libxcb postgresql-devel

./configure -developer-build -opengl -release -nomake tests -xcb -xcb-xlib -no-openssl -no-cups -no-qml-debug -opensource -confirm-license -sql-psql -no-sql-mysql -no-sql-sqlite -no-sql-odbc -skip webengine -prefix /usr/local/Qt-5.15.2
make -j`nproc`
sudo make install
```


# cutelsyst notes

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
