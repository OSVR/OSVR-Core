# OSVR Compatibility Changes

This is an abbreviated changelog, including only those changes that affect one of:

- PluginKit or ClientKit API/ABI
- Library distribution
- Exported CMake-based build system/configuration
- Compatibility between clients and servers

Use git for a full changelog.

Entries are labeled with the `git describe`-derived version of a representative commit and date.

## v0.2 (23-April-2015)
**Affects:** Binding/integration distribution, usage

This is a large update. The main take-aways are that you must update both the client and server due to protocol changes, the number of DLLs has changed, and the config file format has evolved and is much smaller than before.

There are new tools: `osvr_print_tree` displays the contents of the path tree (try `-h` for help with command line arguments), and `PathTreeExport` exports path tree structure to DOT format - see the documentation for usage.

## v0.1-777-g830cc1b (12-March-2015)
**Affects:** Binding/integration distribution

**Main change:** The contents of the `osvrTransform` and `osvrRouting` shared libraries (`.dll` files) have been folded into `osvrCommon` and thus those files are now no longer produced by the build, nor need to be distributed.

This is internal API only, so no changes to apps expected.

## v0.1.728-g47b68e (5-March-2015)
**Affects:** Plugin developers, config files, and install instructions for out of tree plugins

**Main change:** Plugin directory/search path and auto-loading

- Plugins are now stored in a specific directory (on Windows, `/bin/osvr-plugins-0`, with accommodations made to handle MSVC build tree directories; similar location on other platforms except in a `lib` subdirectory) and so have an associated search path, instead of generically on the dynamic loader's search path.
- All dynamic libraries found on the plugin search path are assumed to be plugins, and unless their filename ends in a specific suffix (set up in CMake, defaults to `.manualload`, e.g. `something.manualload.dll`), they will be loaded automatically without having their name listed in the `plugins` portion of the `osvr_server_config.json` file.
- Explicitly-listed plugins in a config file will be searched for in the new plugin-specific directories, both with and without the `.manualload` suffix - so listing a plugin explicitly in the config is now only needed for plugins marked `.manualload`
- The CMake function `osvr_add_plugin` has been overhauled to include support for automatically setting up the appropriate plugin name and install directory. If your plugin is suitable for autoloading, your existing build scripts will work fine, though you may wish to migrate to the new named-parameter syntax for a more concise build.
	- Updated plugin example build system: <https://github.com/OSVR/OSVR-Core/blob/master/examples/plugin/selfcontained/CMakeLists.txt>
	- Documentation of the `osvr_add_plugin` function, adjacent to its source: <https://github.com/OSVR/OSVR-Core/blob/master/osvrAddPlugin.cmake>

## v0.1-658-g5dbf08b (2-March-2015)

**Affects:** CMake-based OSVR consumers

This commit and several preceding it improved the build system for out-of-tree consumption.

- When pointing `osvr_DIR` or `CMAKE_PREFIX_PATH` at a build tree of OSVR-Core, you no longer append `/osvr` to the build directory. You may want to clear out this directory in your build tree to avoid using outdated build materials.
- As the current releases of CMake cannot automatically find the Boost Windows MSVC binaries in their default install location (and where they are installed by the Boxstarter scripts), a helper script is now included with the OSVR CMake config scripts, and will be used to assist the Boost package finder if Boost has not already been found by the time `find_package(osvr)` is called. This should generally make things easier, but if you have an unusual system setup it might be "too helpful" in suggesting where Boost is.

## Previously...

**TODO:** note commits for these

- Imaging merge:
	- Added required shared library for clients and servers: `osvrCommon`
	- Server/client incompatibility: newer servers can be configured to send "string" routes for things that aren't trackers (such as imaging devices) - older clients may react badly if connecting to a server that is configured to provide such routes. Client DLLs may be replaced to handle this.
	- Initial imaging support in PluginKit and ClientKit - limited to 8-bit-per-channel images with a total image size of just under 64KB.

- **PluginKit API/ABI break:** Unification of PluginKit async and sync device callbacks.
