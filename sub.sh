mkdir -p build
cd build

cmake ../lib/detector_c
make -j6 install