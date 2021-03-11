#!/bin/bash
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

pushd "${SCRIPT_DIR}/.."

# build our external dependencies in build/external using external/CMakeLists.txt 
cmake -S external -B build/external
cmake --build build/external --parallel

# build the autoxtime code directly in the build/ directory using ./CMakeLists
cmake -S . -B build
cmake --build build
