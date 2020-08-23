#!/bin/sh
mkdir -p build
cd build
cmake ..
cd ..
make -j 9 -C build && ./build/learn_vulkan
