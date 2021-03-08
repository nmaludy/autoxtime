#!/bin/bash
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
export LD_LIBRARY_PATH="$SCRIPT_DIR/../build/install-prefix/lib64:$SCRIPT_DIR/../build/install-prefix/lib:/usr/local/Qt-5.15.2/lib:$LD_LIBRARY_PATH"
export PATH="$SCRIPT_DIR/../build/install-prefix/bin:$PATH"

cutelyst2 -r --server --app-file $SCRIPT_DIR/../build/src/AutoXTimeServer/src/libAutoXTimeServer.so -- --chdir $SCRIPT_DIR/..
