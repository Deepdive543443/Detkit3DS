# ![icon_detkit](https://github.com/Deepdive543443/Detkit_UI/assets/83911295/6b4f854c-e74b-4065-a4ce-83fce7fa3662) Detkit3DS

![MVIMG_20240223_221721](https://github.com/Deepdive543443/Detkit3DS/assets/83911295/af55da55-4aa9-41e2-b09f-c8a09e860425)
A tiny object detection app for Nintendo 3DS, powered by NCNN and LVGL 8.3.11

## Features 
Detkit3DS is built upon NCNN, LVGL 8.3.11, and my previous project DetkitNCNN.

UI inspired by Nintendo 3DS system UI

None real-time object detection powered by:
- Nanodet-Plus(int8) (~6s)
- Fastest Det (~4s)

## Release
https://github.com/Deepdive543443/Detkit3DS/releases

## Build
This project has dependencies on DevkitARM, NCNN, and LVGL. 
A Linux environment like Ubuntu 20.04 is recommanded to build this project. WSL is also recommanded for windows user.
#### Install DevkitARM toolchain follow the official guide [DevkitPRO](https://devkitpro.org/wiki/Getting_Started)
Or using the guide line from [this repo](https://github.com/Deepdive543443/Benchncnn-3DS)

#### Pull this repo and its submodules, download and build all the submodules
```
sudo apt install unzip cmake
git clone https://github.com/Deepdive543443/Detkit3DS.git --recurse-submodules
cd Detkit3DS
./setup.sh
```
#### Build the executatble
```
cd build
cmake -DBUILD_CIA=ON ..
make -j4
```
Install the .cia using a title manager or running the .3dsx from Homebrew launcher.

## Known issues and limitation
- Still trying to figure how GPU rendering on 3DS works

## Credit
- [NCNN](https://github.com/Tencent/ncnn): High performance neural network inference computing framework for mobile platform, easy to use and port
- [Nanodet-Plus](https://github.com/RangiLyu/nanodet):  Super light weight anchor-free object detection model
- [Fastest-Det](https://github.com/dog-qiuqiu/FastestDet): A FASTER, STRONGER, SIMPLER single scale anchor-free object detection model
- [DevkitPRO](https://devkitpro.org/wiki/Getting_Started): Toolchain for 3DS homebrew development
- [3DS-cmake](https://github.com/Xtansia/3ds-cmake): Toolchain files to build CMake project for 3DS
- [FTPD-Pro](https://github.com/mtheall/ftpd): FTP Server for 3DS/Switch/Linux.
- [Citra](https://github.com/citra-emu/citra): 3DS emulator for Windows, Linux, and MacOS
- [stb](https://github.com/nothings/stb): single-file public domain (or MIT licensed) libraries for C/C++
- All the legendary works and discovering from GBATemp and other hacking communities

