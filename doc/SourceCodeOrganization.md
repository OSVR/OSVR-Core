# Source Code Organization

## Root directory
There are three main code directories in the root of this repository that contribute to the framework's libraries. Two of these directories, `src` and `inc`, have parallel structure.

- `vendor` - contains "vendored-in" third-party libraries in individual folders. These are typically not relevant to consumers of the overall framework.
- `inc` - This directory contains header (`.h`) files intended for some form of external consumption.  Note that the meaning of "external consumption" varies between the libraries that make up the framework, as well as between the different "faces" of libraries that present interfaces to more than one conceptual type of consumer.
    - As a guideline, each header in this directory should compile cleanly when in a translation unit of its own. This guideline is subject to possible testing. (Inspired by [Boost's header policy][boost-header-policy])
- `src` - This directory contains implementation (typically `.cpp`) files to provide the facilities advertised by the interfaces in `inc`. It also contains some header (`.h`) files that provide interfaces that are purely internal within a library. Headers located here (with the exception of `.h.in` files that may be configured to generate API headers in a build or install tree) are not considered part of the API and should typically be ignored by framework consumers as implementation details.

[boost-header-policy]: http://www.boost.org/development/header.html

## Within the parallel interface/implementation trees
Within `inc` and `src`, the top level directory is `ogvr`, serving as an umbrella for all directly-related framework libraries. Within `ogvr`, a number of directories are present, to divide up the interface (and to a lesser degree, the implementation) into logically-distinct parts.

Note that for clarity, a single compiled library might actually implement the interfaces from two directories under `inc/ogvr` in the case that it presents two different interfaces to different types of consumers. In the build system, though only a single output library might be compiled, additional "interface" targets are added to distinguish the interfaces. If this seems confusing, just ignore it as an implementation detail and pretend that each directory under `inc/ogvr` is a library of its own.

- `Util` - Contains shared code useful to the framework but not directly pertaining to any particular facet. Much of this code is header-only: either self-configuring headers wrapping include files, C macro definitions, or C++ template code.

- `PluginKit` - This is the interface presented to device and analysis plugins. For binary compatibility, it is a C API with header-only C++ wrappers.

- `PluginHost` - This is the host of plugins that build against PluginKit. It is primarily consumed by other code in the framework.

- `Connection` - The workings of devices and connections for the framework's messaging system.

## File naming
There are just a few general file name guidelines that might help you find your way around.

- For shared library support, each library has a generated `Export.h` file (that may or may not have a corresponding input file in this source tree) which may not necessarily end up in the documentation due to how it is built. 
- Any file matching the pattern `*C.h` is a C API header file, required to be "C-safe". As such, it must contain only valid C code. Notes:
    - Any included files must also be C-safe.
    - No C++-style (`//`) comments.
    - Everything is wrapped in `extern "C" {}` when in a C++ compiler.
    - A modified guideline for clean compilation applies and may be tested: such headers should be compilable in both a C translation unit and a C++ translation unit.
    - The corresponding implementation in the source directory is _not_ required to be in C, in fact, it's probably in C++, but that shouldn't matter to a framework consumer.
- Implementation files are C++ files, with the same name as the corresponding header but with a `.cpp` extension instead. Yes, this means there are "*C.cpp" files.