SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

function build_module {  
case "$1" in
lvgl)
    cd $SCRIPT_DIR/build
    rm -rf *

    cmake -DCMAKE_TOOLCHAIN_FILE="$SCRIPT_DIR/cmake/DevkitArm3DS-toolchain.cmake" $SCRIPT_DIR/lib/liblvgl
    make -j6 install
    mv install $SCRIPT_DIR/lib/build/lvgl
    rm -rf *

    cd $SCRIPT_DIR
    ;;
ncnn)
    cd $SCRIPT_DIR/build
    rm -rf *

    cmake -DCMAKE_TOOLCHAIN_FILE="$SCRIPT_DIR/cmake/DevkitArm3DS-toolchain.cmake" \
        -DNCNN_SIMPLEOCV=ON \
        -DNCNN_OPENMP=OFF \
        -DNCNN_SIMPLEVK=OFF \
        -DNCNN_VFPV4=OFF \
        $SCRIPT_DIR/lib/ncnn
    make -j6 install
    mv install $SCRIPT_DIR/lib/build/ncnn_3ds
    rm -rf *

    cd $SCRIPT_DIR
    ;;
detkit)
    cd $SCRIPT_DIR/build
    rm -rf *

    cmake -DCMAKE_TOOLCHAIN_FILE="$SCRIPT_DIR/cmake/DevkitArm3DS-toolchain.cmake" \
        -DNCNN_DIR="$SCRIPT_DIR/lib/build/ncnn_3ds/lib/cmake/ncnn" \
        -DBUILD_C=ON \
        $SCRIPT_DIR/lib/detector_c
    make -j6 install
    mv install $SCRIPT_DIR/lib/build/detector
    rm -rf *

    cd $SCRIPT_DIR
    ;;
3dsx)
    cd $SCRIPT_DIR/build
    rm -rf *

    cmake $SCRIPT_DIR
    make -j6 install
    cp $SCRIPT_DIR/build/3dsx/* $SCRIPT_DIR/out

    cd $SCRIPT_DIR
    ;;
cia)
    cd $SCRIPT_DIR/build
    rm -rf *

    cmake -DBUILD_CIA=ON $SCRIPT_DIR
    make -j6 install
    cp $SCRIPT_DIR/build/cia/* $SCRIPT_DIR/out
    ;;
test)
    cd $SCRIPT_DIR/build
    rm -rf *

    cmake -DBUILD_TEST=ON $SCRIPT_DIR
    make -j6 install
    cp $SCRIPT_DIR/build/3dsx/* $SCRIPT_DIR/out
    ;;
all)
    ./setup.sh lvgl ncnn detkit 3dsx cia
    ;;
clean)
    rm -rf $SCRIPT_DIR/build
    rm -rf $SCRIPT_DIR/out
    rm -rf $SCRIPT_DIR/lib/build
    ;;
esac 
}

mkdir -p $SCRIPT_DIR/build
mkdir -p $SCRIPT_DIR/out
mkdir -p $SCRIPT_DIR/lib/build

build_module "$1"
build_module "$2"
build_module "$3"
build_module "$4"
build_module "$5"
