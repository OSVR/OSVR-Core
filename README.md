OpenGoggles VR Plugin Kit
=========================

This is the code and libraries to use when developing OGVR plugins.

Copyright 2014 Sensics, Inc. All rights reserved.

(Intended to be licensed under the Apache License, Version 2.0)

See [the HACKING.md file](HACKING.md) for some development guidelines.


Vendored Projects
-----------------
- `/cmake` - Git subtree from <https://github.com/rpavlik/cmake-modules> used at compile-time only. Primarily BSL 1.0, some items under the license used by CMake (BSD-style)
- `/vendor/gtest-1.7.0` - Unpacked source from <https://code.google.com/p/googletest/> - 3-clause BSD license. Used only for building/running tests.
- `/vendor/qvrpn` - Git subtree, used primarily for one header. BSL 1.0.
    - `git subtree add --squash --prefix vendor/qvrpn https://github.com/rpavlik/qvrpn.git master`
- `/vendor/util-headers` - subset of headers installed from <https://github.com/rpavlik/util-headers>. BSL 1.0.
- `/vendor/vrpn` - Git submodule, accessed from <https://github.com/rpavlik/vrpn>. BSL 1.0. Contains optional recursive submodules HIDAPI and libjsoncpp.

