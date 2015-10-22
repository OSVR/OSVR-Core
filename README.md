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
This snapshot contains documentation, example code, JSON schemas, and JSON descriptor files in the `share/doc/osvrcore` directory. More information is available online.

## Source Documentation
See [the HACKING file](HACKING.md) for some development guidelines for working on this code base itself.

If you're looking at the source tree, not a binary snapshot, you may be interested in [build docs](doc/Building.md). You may also be interested in binary snapshots at <http://osvr.github.io> .

## License and Vendored Projects

This project: Licensed under the Apache License, Version 2.0.

- `/cmake` - Git subtree from <https://github.com/rpavlik/cmake-modules> used at compile-time only. Primarily BSL 1.0, some items under the license used by CMake (BSD-style)
- `/vendor/eigen-3.2.6` - Unpacked release from <http://eigen.tuxfamily.org/> - header-only library under the MPL2 (a file-level copyleft, compatible with proprietary software), define `EIGEN_MPL2_ONLY` to exclude modules with other license from the build (done in OSVR-Core build system).
- `/vendor/gtest-1.7.0` - Unpacked source from <https://code.google.com/p/googletest/> - 3-clause BSD license. Used only for building/running tests.
- `/vendor/jenkins-ctest-plugin` - Git submodule from <https://github.com/rpavlik/jenkins-ctest-plugin>, used only for building/running tests and CI. MIT license.
- `/vendor/util-headers` - subset of headers installed from <https://github.com/rpavlik/util-headers>. BSL 1.0.
- `/vendor/vrpn` - Git submodule, accessed from <https://github.com/vrpn/vrpn>. BSL 1.0. Contains optional recursive submodules HIDAPI and libjsoncpp.
