# ![icon_detkit](https://github.com/Deepdive543443/Detkit_UI/assets/83911295/6b4f854c-e74b-4065-a4ce-83fce7fa3662) Detkit3DS


Detkit3DS is a tiny object detection app for Nintendo 3DS.

## Features 
Detkit3DS is built upon NCNN and LVGL and my previous project DetkitNCNN.

None real-time object detection powered by:
- Nanodet-Plus(int8) (~6s)
- Fastest Det (~4s)


## Build
This app has dependencies on Libctru for SDK, NCNN for neural network inference, and LVGL 8.3.11 for UI backend.
The main working environment of this project is Ubuntu 22.04.1. Depends on the environment you're using, you might need to rebuild part of the buildtools(bannertool, makerom).


Build this project by:
```
mkdir build && cd build
cmake -DBUILD_CIA=ON ..
make -j4
```
Install the .cia using a title manager or running the .3dsx from Homebrew launcher.

## Known issues and limitation
The virtual button driver sometimes cannot be deallocated which lead to some unexpected behaviour

All graphics were rendering on CPU

## Credit
- [NCNN](https://github.com/Tencent/ncnn): High performance neural network inference computing framework for mobile platform, easy to use and port
- [Nanodet-Plus](https://github.com/RangiLyu/nanodet):  Super light weight anchor-free object detection model
- [Fastest-Det](https://github.com/dog-qiuqiu/FastestDet): A FASTER, STRONGER, SIMPLER single scale anchor-free object detection model
- [DevkitPRO](https://devkitpro.org/wiki/Getting_Started): Toolchain for 3DS homebrew development
- [3DS-cmake](https://github.com/Xtansia/3ds-cmake): Toolchain files to build CMake project for 3DS
- [FTPD-Pro](https://github.com/mtheall/ftpd): FTP Server for 3DS/Switch/Linux.
- [Citra](https://github.com/citra-emu/citra): 3DS emulator for Windows, Linux, and MacOS
- All the legendary works and discovering from GBATemp and other hacking communities

