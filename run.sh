#! /bin/bash

pushd build/ > /dev/null
./MIF -width 12 ./Logo.png
cat Logo.png.mif
popd

