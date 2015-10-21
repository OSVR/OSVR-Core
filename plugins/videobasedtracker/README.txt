This directory contains information needed to simulate images from the OSVR HDK, using the original designed LED locations and flash patterns.

Author: Sensics.com
License: Apache 2.0

Usage:
	You need to copy the OpenCV DLLS into the directory with this DLL, or they must be on the path, for the plug-in to work.

simulated_images:
	Simulated images used to test the plug-in.

HDK_random_images:
        Debugging images using the OSVR HDK views from an unsynchronized camera.  They were used to make sure that the blob-finding an size-detection code worked with the flash pattern in use during development in early May 2015.
	See README in simulated_images for how to point at them.

doc:
	Documentation on how to use the OSVR HDK video-based tracking and on how to develop new devices to be compatible with it.

