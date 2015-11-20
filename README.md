# OSVR Core System
> Maintained at <https://github.com/OSVR/OSVR-Core>
>
> For details, see <http://osvr.github.io>
>
> For support, see <http://support.osvr.com>

This is the code and libraries to use when developing with OSVR.

- For developing plugins, you'll use the @ref PluginKit library.
- For developing applications using the framework, you'll use the @ref ClientKit library.

## Binary Snapshot Documentation
If you're looking at this file in a binary build of OSVR-Core, please seen the `BINARY_README` file for more targeted information, as this main README file is focused on the source tree.

## Source Documentation
See [the HACKING file](HACKING.md) for some development guidelines for working on this code base itself.

If you're looking at the source tree, not a binary snapshot, you may be interested in [build docs](doc/Building.md). You may also be interested in binary snapshots at <http://osvr.github.io> .

## License and Vendored Projects

This project: Licensed under the Apache License, Version 2.0.

- `/cmake` - Git subtree from <https://github.com/rpavlik/cmake-modules> used at compile-time only. Primarily BSL 1.0, some items under the license used by CMake (BSD-style)
- `/vendor/com_smart_pointer` - Header (for integrating `boost::intrusive_ptr` with MS COM) extracted from the following project and file: <https://github.com/rengeln/nyx/blob/master/src/Prefix.h>, modified as needed for compatibility. MIT license.
- `/vendor/discount-windows-bins` - Submodule containing source, build script, and Windows binaries for the "Discount" Markdown processor, used only during the build process on Windows. Upstream at <https://github.com/Orc/discount>. 3-clause BSD license.
- `/vendor/eigen-3.2.7` - Unpacked release from <http://eigen.tuxfamily.org/> - header-only library under the MPL2 (a file-level copyleft, compatible with proprietary software), define `EIGEN_MPL2_ONLY` to exclude modules with other license from the build (done in OSVR-Core build system).
- `/vendor/FloatExceptions` - Code for development use only, usage should not be committed or merged into master. MIT licensed.
- `/vendor/gtest-1.7.0` - Unpacked source from <https://code.google.com/p/googletest/> - 3-clause BSD license. Used only for building/running tests.
- `/vendor/jenkins-ctest-plugin` - Git submodule from <https://github.com/rpavlik/jenkins-ctest-plugin>, used only for building/running tests and CI. MIT license.
- `/vendor/libcxx-backports` - C++11/14 standard library functionality backported into headers using the implementations from the libc++ project at <http://libcxx.llvm.org/>. Dual licensed under [the MIT and the "BSD-like" UIUC license](http://llvm.org/docs/DeveloperPolicy.html#license).
- `/vendor/UIforETWbins` - Distributed binary releases of UIforETW <https://github.com/google/UIforETW> extracted to a Git repo (submodule) unmodified. Apache License, Version 2.0.
- `/vendor/util-headers` - subset of headers installed from <https://github.com/rpavlik/util-headers>. BSL 1.0.
- `/vendor/vrpn` - Git submodule, accessed from <https://github.com/vrpn/vrpn>. BSL 1.0. Contains optional recursive submodules HIDAPI and libjsoncpp.
