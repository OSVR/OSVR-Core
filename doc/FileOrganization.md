# File Organization

## Binary snapshot
Snapshots include directories roughly corresponding to the [Filesystem Hierarchy Standard](http://www.pathname.com/fhs/), which should be already familiar to *nix users.

You can add the root directory of a snapshot to your `CMAKE_PREFIX_PATH` when running CMake to help it find the framework when building software using the libraries.

- `*-ver.txt` - Version stamp files: results of `git describe` when run in the library's repository as well as dependencies.
- `bin/` - Executable files, including example clients and servers, as well as:
	- `bin/osvr_server*` - the main server executable (`.exe` on Windows) and its default JSON config file (`_config.json`). Note that you can pass the path to an alternate config file as the first argument to the server.
	- `bin/osvr*.dll`, `bin/osvr-plugins-0/*.dll` - On Windows, the shared library runtime files (`osvr*.dll`) as well as dynamically-loadable plugins are included in the same directory as the executables to simplify search paths for now.
- `include/` - In here you'll find the public header files for all OSVR modules and non-internal dependencies. This corresponds to the `inc` directory, whenever it is discussed later.
- `lib/` or similar - Contains the DLL import libraries (`.lib`) on Windows, and the shared libraries and dynamically-loaded plugins on non-Windows platforms.
	- `lib/cmake/` - In these directories are the CMake config files used by the [`find_package()`][find_package] command to allow you to easily use the OSVR framework from your CMake-based project.
- `share/doc/osvrcore/` - This directory contains documentation and samples. Depending on your snapshot, it may also include a copy of the Doxygen-based API docs. This directory is optional and for human consumption: no compiled OSVR code uses files from this directory.

[find_package]: http://www.cmake.org/cmake/help/v3.0/command/find_package.html

You may now skip the following section and continue, remembering that you have just the `inc` directory contents, located instead in `include`.

## Source repository
There are three main code directories in the root of this repository that contribute to the framework's libraries. Two of these directories, `src` and `inc`, have parallel structure.

- `vendor` - contains "vendored-in" third-party libraries in individual folders. These are typically not relevant to consumers of the overall framework.
- `inc` - This directory contains header (`.h`) files intended for some form of external consumption.  Note that the meaning of "external consumption" varies between the libraries that make up the framework.
    - As a guideline, each header in this directory should compile cleanly when in a translation unit of its own. (Inspired by [Boost's header policy][boost-header-policy])  (Enforced by build system "header dependency test" targets)
- `src` - This directory contains implementation (typically `.cpp`) files to provide the facilities advertised by the interfaces in `inc`. It also contains some header (`.h`) files that provide interfaces that are purely internal within a library. Headers located here (with the exception of `.h.in` files that may be configured to generate API headers in a build or install tree) are not considered part of the API and should typically be ignored by framework consumers as implementation details.

[boost-header-policy]: http://www.boost.org/development/header.html

## File naming
There are just a few general file name guidelines that might help you find your way around.

- For shared library support, each library has a generated `Export.h` file which may not necessarily end up in the documentation due to how it is built.
- Headers matching the pattern `*_fwd.h` are header files forward-declaring whatever items are more fully declared or defined in the corresponding main header without the trailing `_fwd`. To avoid mis-matches, these should always be included in the main header. (Enforced by a test)
- Similarly, headers matching the pattern `*Ptr.h` typically contain a forward declaration as well as a smart pointer typedef intended for use with the corresponding main header. These should likewise be included in the main header. (Enforced by a test)
- Any file matching the pattern `*C.h` is a C API header file, required to be "C-safe". As such, it must contain only valid C code. Notes:
    - Any included files must also be C-safe.
    - No C++-style (`//`) comments.
    - Everything is wrapped in `extern "C" {}` when in a C++ compiler.
    - A modified guideline for clean compilation applies and may be tested: such headers should be compilable in both a C translation unit and a C++ translation unit. (Enforced by build system "header dependency test" targets)
    - The corresponding implementation in the source directory is _not_ required to be in C, in fact, it's probably in C++, but that shouldn't matter to a framework consumer.
- Implementation files are C++ files, with the same name as the corresponding header but with a `.cpp` extension instead. Yes, this means there are "*C.cpp" files.

## Within the parallel interface/implementation trees
Within `inc` and `src`, the top level directory is `osvr`, serving as an umbrella for all directly-related framework libraries. Within `osvr`, a number of directories are present, to divide up the interface and  the implementation into logically-distinct parts.

These follow in alphabetical order. For logically-ordered documentation, see the Modules link on the left.

- `Client` - Internal API: Implementation details internal to the client side. Provides the implementation that ClientKit effectively wraps. (See @ref Client)

- `ClientKit` - The C API (with C++ header-only wrappers) used by client applications: games, etc. (See @ref ClientKit)

- `Connection` - Internal API: The workings of devices and connections for the framework's messaging system. (See @ref Connection)

- `PluginHost` - Internal API: This is the host for plugins that build against @ref PluginKit. It is primarily consumed by other code in the framework. (See @ref PluginHost)

- `PluginKit` - This is the interface presented to device and analysis plugins. For binary compatibility, it is a C API with header-only C++ wrappers. (See @ref PluginKit)

- `Common` - Internal API: Shared internal workings between client and plugin/server sides (See @ref Common).

- `Server` - Internal API: A higher-level library used to develop server applications, wrapping connection and plugin handling. (See @ref Server)

- `Util` - Contains shared code useful to the framework but not directly pertaining to any particular facet. Much of this code is header-only: either self-configuring headers wrapping include files, C macro definitions, or C++ template code. (See @ref Util)

Note that in the build system, though only a single output library might be compiled, additional "interface" targets are may be added to allow optional configuration. The best example is that of the optional C++ header-only wrappers: CMake pretends there's another library that links to the un-wrapped library, and adds the required includes for the C++ headers. If this seems confusing, just ignore it as an implementation detail and remember that each directory under `inc/osvr` essentially is a library of its own.
