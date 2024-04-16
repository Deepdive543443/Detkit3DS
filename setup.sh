SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

function build_module {  
case "$1" in
lvgl)
    cd $SCRIPT_DIR/build
    rm -rf *
    wget https://github.com/lvgl/lvgl/archive/refs/tags/v8.3.11.zip
    unzip v8.3.11.zip
    mv lvgl-8.3.11 $SCRIPT_DIR/lib/lvgl/lvgl-8.3.11
    
    cmake -DCMAKE_TOOLCHAIN_FILE="$SCRIPT_DIR/cmake/DevkitArm3DS-toolchain.cmake" $SCRIPT_DIR/lib/lvgl
    make -j6 install
    mv install $SCRIPT_DIR/lib/build/lvgl
    rm -rf *
    ;;

ncnn)
    mkdir -p $SCRIPT_DIR/lib/build/ncnn_3ds
    cd $SCRIPT_DIR/lib/build/ncnn_3ds
    wget https://github.com/Deepdive543443/Benchncnn-3DS/releases/download/20240410/ncnn_3ds_20240410.zip
    unzip ncnn_3ds_20240410.zip
    rm ncnn_3ds_20240410.zip
    ;;

detkit)
    cd $SCRIPT_DIR/build
    rm -rf *
    cmake -DCMAKE_TOOLCHAIN_FILE="$SCRIPT_DIR/cmake/DevkitArm3DS-toolchain.cmake" -DNCNN_DIR="$SCRIPT_DIR/lib/build/ncnn_3ds/lib/cmake/ncnn" $SCRIPT_DIR/lib/detector_c
    make -j6 install
    mv install $SCRIPT_DIR/lib/build/detector
    rm -rf *
    ;;

clean)
    rm -rf $SCRIPT_DIR/build
    rm -rf $SCRIPT_DIR/lib/build
esac 
}

mkdir -p $SCRIPT_DIR/build
mkdir -p $SCRIPT_DIR/lib/build

build_module "$1"
build_module "$2"
build_module "$3"
# if [ "$" == "" ] 
# then
#   build_module("")
#   build_module(lvgl)
#   build_module(ncnn)
#   build_module(detkit)
# else
#   build_module(detkit)
# fi
