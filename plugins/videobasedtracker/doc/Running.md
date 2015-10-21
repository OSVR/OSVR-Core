# Using the OSVR HDK's video-based tracker

This document describes how to use the OSVR HDK's video-based tracker, including how to plug in the camera and run a server that will use it.  See the [development document](./Developing.md) for how to develop new peripherals that would make use of the HDK camera to track objects in the scene.

## Set-up

The video-based tracking module uses a custom infrared camera that plugs into the server computer via USB  and into the HDK belt-pack via a phono connector.  It is run using using the standard OSVR server with a different configuration file that causes it to load a plug-in to do video-based tracking using the camera.

### Plugging in the unit

The camera ships with a small three-legged tripod that can be used to position and aim it at the field of view you want to track.  The tripod screws into the bottom of the camera and has a freely-rotating ball joint that can be tightened to point the camera.

First plug the USB cable, shipped with the HDK, into the micro-USB port on the side of the camera.  Then plug the USB cable into a free USB connector on the server computer.  Windows should recognize the camera as an imaging device and install it into the system.

![USB to camera](./camera_USB.png)

Next, plug the phono cable that is attached to the power-supply connector into the camera.  Note: Be sure to plug the connector **all the way in** so that the edge of the connector is right up against the side of the camera.  The connector will go in easily most of the way and then snaps in after a final push.

![Camera both plugs](./camera_plugs.png)

Finally, plug the synchronizing power adapter.  This is shown disconnected in the image below.  The power adapter plugs into the left end of the cable and the right end of the cable plugs into the belt pack.  This inserts both power and the camera synchronization signal into the belt back, and from there into the HDK.  Plug the power adapter into the wall outlet to power the HDK and video-based tracking should be ready to go.

![Synchronized power plug](./sync_power_adapter.png)

### Running the server

Once everything is plugged in, the standard OSVR server can be used to control it.  A special configuration file is required to tell the server to load the video-based tracking code.  On Windows, you can drag the file named *osvr_server_config.VideoBasedHMDTracker.sample.json* and drop it onto the *osvr_server.exe* program, or you can open up a Windows command shell and go to the directory where the server lives and then run the following:

    osvr_server.exe osvr_server_config.VideoBasedHMDTracker.sample.json

In the Cygwin shell and other Linux-like shells, you may need to explicitly tell it to us the one in the current directory.

    ./osvr_server.exe osvr_server_config.VideoBasedHMDTracker.sample.json

The server should report that it added a device named something like *com_osvr_VideoBasedHMDTracker/TrackedCamera0_0*, which it will automatically map to */me/head* in the OSVR semantic paths.

## Testing and debugging

Once everthing is plugged in and the server is running, you can use any client application to see how it is working.  One application we suggest you try that's handy for checking to make sure that things are working is called *Tracker Viewer*. On the [Using OSVR](http://osvr.github.io/using/) page, you'll see a download link for the OSVR Tracker Viewer. Download and run that (with the OSVR Server running!), and you'll get a small window with some 3D arrows in it. If you're 3D-graphics savvy or VR-savvy, you'll probably figure out what they are and what they mean, but the important part in general is to just verify that the small arrows in the middle move when you rotate the headset. (You can right-click and drag to zoom in to see it better)

### Debugging

If you want to see what is going on under the covers, or if you're not getting any tracker data and want to see what may be wrong, you can... hold on just a few days... until we have a parameter in the configuration file that lets you turn on video windows showing debug info on the tracking system.

## Coming Soon

Sensics is developing an analysis plug-in that will combine video + intertial tracker reports into a single tracker state that includes estimates on velocity and acceleration.

