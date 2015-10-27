# ZipPicViewWx

![Imgur](http://i.imgur.com/oeo5aZV.png)

ZipPicViewWx is an in-zip-archive image viewer. It is written in C++ and runs on Linux and Windows (OSX isn't tested yet). It has only a basic feature like image listing, view with zoom-in/out (up-to 200%), and does not aim to replace your day-to-day image viewer.

![Imgur](http://i.imgur.com/7vQdywe.png)

ZipPicViewWx has been tested on Windows 8.1 and Arch Linux.

## Build

The tested method is to use Unix toolchain. ZipPicViewWx use CMake for build system.

On Windows it has been built on MSYS2 platform on Windows 8.1. There are some difficulties using Cmake on MSYS2 with MINGW64 version of wxWidgets in other versions of Windows (ie. Windows 7). The output, however, should work with Windows 7 onwards. 

ZipPicViewWX depends on [wxWidgets](https://www.wxwidgets.org/) and [libzip](http://www.nih.at/libzip/). It has been compiled on GCC and Clang.
