#!/bin/bash
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
export LD_LIBRARY_PATH="$SCRIPT_DIR/../extern/lib64:$SCRIPT_DIR/../extern/lib:/usr/local/Qt-5.12.2/lib:$LD_LIBRARY_PATH"
export PATH="$SCRIPT_DIR/../extern/bin:$LD_LIBRARY_PATH"

cutelyst2 -r --server --app-file $SCRIPT_DIR/../build/src/autoxtime/AutoXTimeServer/src/libAutoXTimeServer.so -- --chdir $SCRIPT_DIR/..
