SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

mkdir -p build
cd build
wget https://github.com/lvgl/lvgl/archive/refs/tags/v8.3.11.zip
unzip v8.3.11.zip
mv lvgl-8.3.11 $SCRIPT_DIR/lib/lvgl/lvgl-8.3.11

mkdir -p $SCRIPT_DIR/lib/build
wget https://github.com/Deepdive543443/detector_c/releases/download/20240229_master/ncnn_3ds_master.zip
unzip ncnn_3ds_master.zip
mv ncnn_3ds_master $SCRIPT_DIR/lib/build/ncnn_3ds_master

cmake -DCMAKE_TOOLCHAIN_FILE="$SCRIPT_DIR/cmake/DevkitArm3DS-toolchain.cmake" -DNCNN_DIR="$SCRIPT_DIR/lib/build/ncnn_3ds_master/lib/cmake/ncnn" $SCRIPT_DIR/lib/detector_c
make -j6 install

rm -rf *
cmake -DCMAKE_TOOLCHAIN_FILE="$SCRIPT_DIR/cmake/DevkitArm3DS-toolchain.cmake" $SCRIPT_DIR/lib/lvgl
make -j6 install

rm -rf *
