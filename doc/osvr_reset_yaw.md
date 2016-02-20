# OSVR Reset Yaw              {#OSVRResetYaw}

This tool can be used to apply a short-term (lasts only as long as the OSVR Server is running) re-alignment of the `/me/head` coordinate system, changing which direction is nominally "forward".  It works best/is primarily intended for use with orientation-only trackers: there should be little to no need for such a tool with full pose trackers that use outside fixed landmarks or cameras with known locations, as these known locations can establish a consistent desired coordinate system. Further, its

## How to use it

The OSVR server should be running and properly receiving tracking data before you begin. Applications may be left running: they will be updated immediately.

- Start the Reset Yaw application. It will connect to the OSVR Server and wait a few seconds to get quality tracking data before prompting you to continue.
- The application will prompt you to "place your device in its zero orientation and press enter": this is when you should face the direction you want to establish as "forward" (or hold the device as if you were doing so), then press enter to continue.
- The application will record the orientation at the time of your enter press, compute the relative rotation about the vertical (Y) axis (that is, yaw) between that orientation and a "default" orientation, and send the OSVR server a message to update its `/me/head` alias to include a transform layer that counteracts that rotation, resulting in this new orientation becoming the effective new forward direction for `/me/head`.
- When all computation and server communication is complete, the application will prompt you again to press enter to exit. Changes take effect immediately.

If you're unsatisfied, you can run the process again: the old correction will be removed first before creating a new one. All running OSVR applications should immediately update to show the effects of a new transform as soon as it is made.

## Limitations
The correction transformations are not saved anywhere besides the operating state of the current server process, so they are lost when you close OSVR Server. (Many of the IMU devices this is designed to work with would require a different such correction at each usage, so this is a reasonable mode of operation.)

The effects are only applied to the alias `/me/head`, and not to any other trackers or sensors that may or may not be in the same coordinate system as `/me/head` (another reason why it's best-suited for use with single-orientation-tracker systems).

As the name suggests, it is specialized to only correct yaw/heading: the ground plane is assumed to be correct. If tracking is mixed up in its transformation beyond just a simple re-pointing of the forward heading/yaw, this tool is the wrong one to use.
