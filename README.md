# OSVR Core System

This is the code and libraries to use when developing with OSVR.

- For developing plugins, you'll use the @ref PluginKit library.
- For developing applications using the framework, you'll use the @ref ClientKit library.

**For support, contact <support@opengoggles.org>.**

Copyright 2014 Sensics, Inc. All rights reserved.

(Intended to be licensed under the Apache License, Version 2.0)

## Binary Snapshot Documentation
This snapshot contains documentation, example code, JSON schemas, and JSON descriptor files in the `share/doc/osvrcore` directory.

That directory may also contain an `api` subdirectory with generated API documentation. If your snapshot does not include this API documentation, [contact us](mailto:support@opengoggles.org?subject=Access%20to%20API%20documentation%20site) for access to a live-updated web site with this documentation.

## Source Documentation
See [the HACKING file](HACKING.md) for some development guidelines for working on this code base itself.

If you're looking at the source tree, not a binary snapshot, you may be interested in [build docs](docs/Building.md). You may also be interested in [contacting us for access to binary snapshots](mailto:support@opengoggles.org?subject=Access%20to%20binary%20snapshots)

## Vendored Projects

- `/cmake` - Git subtree from <https://github.com/rpavlik/cmake-modules> used at compile-time only. Primarily BSL 1.0, some items under the license used by CMake (BSD-style)
- `/vendor/eigen-3.2.2` - Unpacked release from <http://eigen.tuxfamily.org/> - header-only library under the MPL2 (a file-level copyleft, compatible with proprietary software), define `EIGEN_MPL2_ONLY` to exclude modules with other license from the build.
- `/vendor/gtest-1.7.0` - Unpacked source from <https://code.google.com/p/googletest/> - 3-clause BSD license. Used only for building/running tests.
- `/vendor/jenkins-ctest-plugin` - Git submodule from <https://github.com/rpavlik/jenkins-ctest-plugin>, used only for building/running tests and CI. MIT license.
- `/vendor/util-headers` - subset of headers installed from <https://github.com/rpavlik/util-headers>. BSL 1.0.
- `/vendor/vrpn` - Git submodule, accessed from <https://github.com/vrpn/vrpn>. BSL 1.0. Contains optional recursive submodules HIDAPI and libjsoncpp.

