#!/usr/bin/env bash

# If project not ready, generate cmake file.
if [[ ! -d build ]]; then
    echo "good"
else
    rm -rf build
fi
cmake -B build
cmake --build build

# Run all testcases. 
# You can comment some lines to disable the run of specific examples.
mkdir -p output
# build/PA1 testcases/scene01_basic.txt output/scene01.bmp
# build/PA1 testcases/scene02_cube.txt output/scene02.bmp
# build/PA1 testcases/scene03_sphere.txt output/scene03.bmp
# build/PA1 testcases/scene04_axes.txt output/scene04.bmp
# build/PA1 testcases/scene05_bunny_200.txt output/scene05.bmp
# build/PA1 testcases/scene06_bunny_1k.txt output/scene06.bmp
# build/PA1 testcases/scene07_shine.txt output/scene07.bmp
# build/PA1 testcases/scene08_core.txt output/scene08.bmp
# build/PA1 testcases/scene09_norm.txt output/scene09.bmp
# build/PA1 testcases/scene10_wineglass.txt output/scene10.bmp
# build/PA1 testcases/ppm.txt output/ppm.bmp
# build/PA1 testcases/smallpt.txt output/zg_qu.bmp
# build/PA1 testcases/water2.txt output/water2.bmp
build/PA1 testcases/zg_len.txt output/zg_len.bmp
# build/PA1 testcases/tuzi__.txt output/zg5.bmp
# build/PA1 testcases/mov.txt output/mov_zg7.bmp
# build/PA1 testcases/ppm.txt output/ppm_2.bmp

