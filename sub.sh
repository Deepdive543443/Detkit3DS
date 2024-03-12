mkdir -p build
cd build

cmake  -DCMAKE_TOOLCHAIN_FILE="../cmake/DevkitArm3DS.cmake" ../lib/detector_c
make -j6 install