#! /bin/bash

CFLAGS="-Wall -Wextra -fno-exceptions -fno-rtti"
CFLAGS_WARN="-Wno-unused-parameter"
CFLAGS_DEBUG="-g -DDEBUG"
CFLAGS_RELEASE="-O3"

CXX=clang++

mkdir -p build/

$CXX $CFLAGS $CFLAGS_WARN $CFLAGS_RELEASE src/MIF.cpp -o build/MIF

cp src/*.png build/

