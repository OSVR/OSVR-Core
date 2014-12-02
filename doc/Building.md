# Building the Libraries

You'll need:

- A relatively recent version of Boost (1.43 or newer)
- [libfunctionality][] compiled
- [jsoncpp][] compiled, with the `JSONCPP_WITH_CMAKE_PACKAGE` option turned on, and the `JSONCPP_LIB_BUILD_SHARED` option turned off on Windows and on for other platforms.
- A C++11 capable compiler: VS 2013 is the main one used in development. GCC in the nuwen distro of MinGW also works.

For Linux:

- The above (any reasonably recent Clang or GCC is fine), plus `libusb-1.0-0-dev`

[libfunctionality]:https://github.com/sensics/libfunctionality
[jsoncpp]:https://github.com/sensics/jsoncpp