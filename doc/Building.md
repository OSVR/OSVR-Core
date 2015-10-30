# Building the Libraries {#Building}

See the OSVR Wiki page on [Getting Started with Core Development][wiki-getting-started] for more step-by-step instructions, including links to pre-compiled Windows binary dependencies.

[wiki-getting-started]:http://wiki.osvr.com/doku.php?id=startingcore

The rest of the page you're currently reading is targeted at those interested in in-depth development (particularly across platforms), so it leaves a lot of blanks that it assumes you can fill in.

For Windows: 

You'll need:

- Git (make sure to clone OSVR-Core with `--recursive` flag to get all submodules)
- CMake, fairly recent (3.0 or newer, 3.0.2 most tested, that or the latest preferred.)
- A relatively recent version of Boost libraries(1.44 or newer, 1.57.0 on Windows the most tested, the latest is usually preferred barring any weird bugs which will be noted here if discovered.)
    - 1.58.0 is not compatible due to a [bug in Boost](http://lists.boost.org/Archives/boost/2015/05/221933.php), the fix will be included in 1.58.1
- [libfunctionality][] compiled
- [jsoncpp][] compiled, with the `JSONCPP_WITH_CMAKE_PACKAGE` option turned on, and the `JSONCPP_LIB_BUILD_SHARED` option turned off on Windows and on for other platforms.
- OpenCV 2.4.x - there is some initial work on porting to support 3.0 but it's not widely tested.
- A C++11 capable compiler: VS 2013 is the main one used in development. (VS 2012 doesn't support enough C++11 to build the core currently.) MSYS2 with MinGW64 GCC and GCC in the nuwen distro of MinGW also work, at least as of some time in the recent past. CI builds regularly on Linux using GCC and Clang, so ideally GCC in MinGW should also stay working, or at least not very broken.

For Linux:

You'll need:

- Git (make sure to clone OSVR-Core with `--recursive` flag to get all submodules)
- Any reasonably recent Clang, GCC/G++ is fine for a compiler:
- CMake, fairly recent (3.0 or newer, 3.0.2 most tested, that or the latest preferred.) Depending on your Linux distribution, package manager may or may not have version 3.0+, so you can get it from <http://CMake.org> or backports/PPA.
- [libfunctionality][] is standalone, so you will need to build it first
- [jsoncpp][], you will need to compile it with `-DJSONCPP_WITH_CMAKE_PACKAGE=ON` in order to generate CMake project config files. Optionally you can make it a static library with ` -DJSONCPP_LIB_BUILD_SHARED=OFF -DCMAKE_CXX_FLAGS=-fPIC` flags (don't forget to `make install` after building) 
- OpenCV 2.4.x
- Boost libraries (1.44 or newer, see note above about bug in Boost 1.58.0), which you can get through package manager. You don't need all of them but you'll need at least : `libboost1.xx-dev, libboost-thread1.xx-dev (includes required system, date-time and chrono), libboost-program-options1.xx-dev, libboost-filesystem1.xx-dev)`
- `libusb1` - this is the Debian package `libusb-1.0-0-dev`. Do not confuse it with `libusb` aka `libusb0`: those packages are frozen in the past and incompatible with `libusb1`.

[libfunctionality]:https://github.com/OSVR/libfunctionality
[jsoncpp]:https://github.com/VRPN/jsoncpp
