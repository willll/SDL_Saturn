#!/bin/bash
set -e

BUILT_FOLDER=./bin

rm -rf $BUILT_FOLDER/* \
      && cmake -B $BUILT_FOLDER \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_TOOLCHAIN_FILE=$SATURN_CMAKE/sega_saturn.cmake \
      -DCMAKE_INSTALL_PREFIX=/saturn/SDL . || exit 1

make VERBOSE=1 -f Makefile -C $BUILT_FOLDER || exit 1

make VERBOSE=1 -f Makefile -C $BUILT_FOLDER install || exit 1

exit 0
