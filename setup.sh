mkdir -p build
cd build
wget https://github.com/lvgl/lvgl/archive/refs/tags/v8.3.11.zip
unzip v8.3.11.zip
mv lvgl-8.3.11 ../lib/lvgl-8.3.11

wget https://github.com/Deepdive543443/detector_c/releases/download/20240229_master/ncnn_3ds_master.zip
unzip ncnn_3ds_master.zip
mv ncnn_3ds_master ../lib/ncnn_3ds_master

cmake -DCMAKE_TOOLCHAIN_FILE="../cmake/DevkitArm3DS-toolchain.cmake" ../lib/detector_c
make -j6 install

rm -rf *
cmake -DCMAKE_TOOLCHAIN_FILE="../cmake/DevkitArm3DS-toolchain.cmake" ../lib/lvgl
make -j6 install

rm -rf *
