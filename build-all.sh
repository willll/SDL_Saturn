#!/bin/bash
set -e

# SDL lib
BUILT_FOLDER=./bin
SDL_PATH=/saturn/install/SDL

rm -rf $BUILT_FOLDER/* \
      && cmake -B $BUILT_FOLDER \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_TOOLCHAIN_FILE=$SATURN_CMAKE/sega_saturn.cmake \
      -DCMAKE_INSTALL_PREFIX=$SDL_PATH . || exit 1

make VERBOSE=1  -f Makefile -C $BUILT_FOLDER || exit 1

make VERBOSE=1 -j -f Makefile -C $BUILT_FOLDER install || exit 1

#SDL samples
rm -rf $BUILT_FOLDER/* \
      && cmake -B $BUILT_FOLDER \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_MODULE_PATH=$CMAKE_MODULE_PATH \
      -DSDL_Saturn_LIBRARY=$SDL_PATH/lib \
      -DSDL_Saturn_INCLUDE_DIR=$SDL_PATH/include \
      -DCMAKE_TOOLCHAIN_FILE=$SATURN_CMAKE/sega_saturn.cmake \
      -DCMAKE_INSTALL_PREFIX=$SDL_PATH/test test || exit 1

make VERBOSE=1 -f Makefile -C $BUILT_FOLDER || exit 1

make VERBOSE=1 -f Makefile -C $BUILT_FOLDER install || exit 1

exit 0
