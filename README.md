# ZipPicViewWx

![Imgur](http://i.imgur.com/4DRWcjM.png)

ZipPicViewWx is an in-zip-archive image viewer. It is written in C++ and runs on Linux and Windows (OSX isn't tested yet). It has only a basic feature like image listing, view with zoom-in/out (up-to 200%), and does not aim to replace your day-to-day image viewer.

![Imgur](http://i.imgur.com/p3i5G5w.png)

ZipPicViewWx has been tested on Windows 10 and Arch Linux.

## Building

The tested method is to use UNIX toolchain. ZipPicViewWx use CMake for build system.

1. Checkout the code. For this guide the code is checked out to `~/ZipPicViewWx`
2. Create a new build directory. For this guide the build directory is `~/ZipPicViewWx-build`.
3. Change directory into the build directory. And then run `$ cmake ~/ZipPicViewWx`.
4. Then run `$ make`. The program is now built.

On Windows, [MSYS2](https://msys2.github.io/) is the preferred method. Install MSYS2 and necessary toolchain, then follow the same instruction as the UNIX toolchain. The Generator has to be specified in step 3, by running `$ cmake -G "MSYS Makefiles ~/ZipPicViewWx"`. 

After the program is built, `cpack` (which is part of CMAKE distribution) can be used to created installer. `ZIP` and `WIX` generators have been tested and used. To create installer package with `cpack`, run the command `$ cpack -G WIX` or `$ cpack -G ZIP` inside the build directory.

To specify the version number into the build, pass `CPACK_PACKAGE_VERSION_MAJOR` and `CPACK_PACKAGE_VERSION_MAJOR` variable during step 3. For example `$ cmake -G "MSYS Makefiles" -DCPACK_PACKAGE_VERSION_MAJOR=0 -DCPACK_PACKAGE_VERSION_MINOR=7 ~/ZipPicViewWx`. By default the version is `DEV.0.0`

ZipPicViewWX depends on [wxWidgets](https://www.wxwidgets.org/) and [libzip](http://www.nih.at/libzip/). It has been compiled on GCC and Clang.
