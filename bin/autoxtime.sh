#!/bin/bash
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
export LD_LIBRARY_PATH="/usr/local/Qt-5.12.2/lib:$LD_LIBRARY_PATH"
"${SCRIPT_DIR}/../build/autoxtime"
