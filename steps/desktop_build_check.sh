#!/bin/bash

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# build
cd $SCRIPT_DIR/..
if [ "$1" == "--rebuild" ]; then
    rm -rf build_qt
    mkdir build_qt
else
    mkdir -p build_qt
fi
cd build_qt
cmake .. -GNinja
ninja

# run tests
ctest -j $(nproc)

# get coverage
echo "Running lcov"
lcov --capture --directory src/decn --output-file coverage.info
lcov --remove coverage.info "/usr/*" --output-file coverage.info
genhtml coverage.info --output-directory lcov
echo "Running gcov"
gcov -b src/decn/CMakeFiles/decn_cover.dir/decn.c.gcno

