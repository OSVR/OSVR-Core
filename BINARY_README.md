# OSVR Core System - Binary Snapshot Readme
> Maintained at <https://github.com/OSVR/OSVR-Core>
>
> For details and more documentation, see <http://osvr.github.io/contributing/#headingOSVR-Core>
>
> For support, see <http://support.osvr.com>

This is the code and libraries to use when developing with OSVR. It also includes the compiled run-time components, like OSVR Server.

- For developing plugins, you'll use the @ref PluginKit library.
- For developing applications using the framework, or for integrating OSVR with a game engine, you'll use the @ref ClientKit library.
- For developing applications with an existing engine integration, you'll want to see that integration's documentation, since this is a lower-level library than you'll likely have to interact with.

## Files and Directories

Helper scripts, in the root directory of Windows builds:

- `add_sdk_to_registry.{cmd,ps1}` - A convenience script for registering the location of an installed build with CMake, so that you can build plugins or applications against OSVR using CMake without having to manually specify the location of the OSVR-Core build. Optional, only used by developers directly using the OSVR-Core APIs in a C or C++ plugin or application. Not used by any game engine integration. *The `.cmd` file takes care of launching the PowerShell script (`.ps1` file) correctly. Just run the `.cmd` file as a regular user if you need this functionality.*

Version stamp files, in the root directory of automatically-built Windows snapshots:

- `osvr-ver.txt` - the `git describe --tags` output of the OSVR-Core source tree used to build the snapshot.
- `libfunctionality-ver.txt` - the `git describe --tags` output of the "libfunctionality" (a dependency of the server) source code used to build the snapshot.

Documentation, etc.: in the root directory on Windows, and `share/doc/osvrcore` on other platforms:

- `BINARY_README.{md,html}` - This file, in Markdown text and/or HTML format.
- `CONTRIBUTING.{md,html}` - the Contributing Guidelines from the source, in Markdown text and/or HTML format.
- `NEWS.{md,html}` - An abbreviated log of breaking changes, in Markdown text and/or HTML format.
- `README.{md,html}` - The README file from the source tree, in Markdown text and/or HTML format.
- `NOTICE` and `LICENSE` - Files related to the Apache License v2.0

In automatically-built Windows snapshots:

- `tracing/` directory - Contains ETW tracing-enabled binaries, see the README in that directory for more information.

In all installed builds:

- `bin/` directory - Contains most-commonly-used executables, including `osvr_server` and tools like `osvr_print_tree`. On Windows, also contains the shared library (`.dll`) files.
- `osvr-plugins-0/` directory, under `bin/` on Windows and under the library directory (often `lib/`) on other platforms - Contains the installed plugins bundled with OSVR-Core. Those whose filename ends in `.manualload` must be explicitly specified in a config file to be loaded; all others automatically load. *Place any additional plugins here*, with any of their required shared-library dependencies (`.dll`/`.so`/`.dylib`) alongside the OSVR shared libraries (in `bin/` or the library directory, depending on platform)
- `share/doc/osvrcore` directory - Contains example code, JSON schemas, JSON descriptors, and possibly documentation.
- `share/osvrcore/extra-sample-apps` directory - Contains the additional sample applications compiled but not placed in `bin`. May need to be placed in `bin` to run correctly.
- Sample configs and descriptors: all under `bin/` on Windows, under `share/osvrcore/` on other platforms
    - `osvr_server_config.json` - Default config file, for fully-auto-detected configurations such as the OSVR HDK.
    - `sample-configs/` directory - Sample server config files.
    - `external-devices/` directory - Sample server config files and device descriptors (in the `device-descriptors/` subdirectory) for using VRPN-supported devices with OSVR.
    - `displays/` directory - Display descriptors for a variety of HMDs, to reference from server config files.
- `lib/` or a similar directory (may vary in multiarch Linux distributions) - On Windows, contains the import libraries used by MS Visual C++ to link against the DLLs. On other platforms, contains the shared libraries themselves. On all platforms, contains any static libraries distributed with OSVR, as well as CMake config filfes.
- `include/` directory - Contains the OSVR header files. Note that all headers, not just those of the "public" or "external" APIs, are included by default for convenience, and may typically be ignored.
