Self-Contained Plugin Example
=============================

This directory contains an example of how to build a plugin in a separate source tree (not integrated with this repository). The `CMakeLists.txt` file controls the build, while the `.cpp` file contains the actual code of the plugin. For your plugin, make a copy of this directory to start. You'll want to change the name:

1. Rename the `.cpp` file.
2. Edit the `.cpp` file to replace the old name in the `OSVR_PLUGIN()` line.
3. Edit the `CMakeLists.txt` file to replace the old name in the target name and sources list.

Then, edit your plugin and build file as needed!
