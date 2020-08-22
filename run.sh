#!/bin/sh
mkdir -p build
echo "-> building..."
cd build
cmake ..
cd ..
make -j 9 -C build && ./build/learn_vulkan
