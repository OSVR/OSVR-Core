# Writing a basic device plugin              {#TopicWritingDevicePlugin}

Writing a device plugin means you'll be building against the PluginKit library, which presents a C API, as well as some C++ header-only wrappers. The API does not impose a required structure, so you can more easily integrate it with an existing driver codebase. However, the following steps can help you build a plugin from scratch.

1. Make a copy of the directory `/examples/plugin/selfcontained` as the starting point for your new plugin repository.
  - This assumes that your plugin will be "out-of-tree" - that is, not hosted in the main Core repository.
2. Do what it says in that `README.md` file to change the plugin name and build system.
3. Implement a hardware poll callback (to determine if your device is connected):
  - This can take the form of a class with a function call operator (`operator()`), taking `OGVR_PluginRegContext ctx` and returning `OGVR_ReturnCode`.
  - See `/examples/plugin/org_opengoggles_example_DummyHardwarePollCpp2.cpp` for a sample.
  - See lines 47-51 to see how to create and register an instance of that class.
  - From within your callback, if the device exists, create a device token (see the `DummyAsync` or `DummySync` examples like `org_opengoggles_example_DummyAsync.cpp`).
4. In the hardware poll callback, do lines 63-72 of the DummyAsync file (perhaps skip line 70 if you are using an existing message type).
5. You need to have your device hold on to the device token to send data with it later on.
6. The guts of implementing a device are in line 51 of DummyAsync: wait until there is data then call `ogvrDeviceSendData()`. (There are two versions of this function: one where you provide a timestamp, and one where the timestamp is automatically created for you.)
7. To test: there is a bundled application called `BasicServer`.  You can start it and pass names of plugins on the command line. This will load the plugin, do a hardware poll, and enter the run loop.
  - You can use `vrpn_print_messages localhost` to see the messages being sent.

Note that these instructions are only for drivers that can fully self-configure: manually configuring drivers is not available yet.