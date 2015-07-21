This directory contains information needed to simulate images from the OSVR HDK, using the original designed LED locations and flash patterns.

Author: Sensics.com
License: Apache 2.0

Usage:
	You need to copy the OpenCV DLLS into the directory with this DLL, or they must be on the path, for the plug-in to work.
	VBHMD_FAKE_IMAGES can be defined in ../com_osvr_VideoBAsedHMDTracker.cpp and pointed at a directory containing the images simulated below, it will use those in a loop to test the tracking alorithm.
	If the fake images is pointed at the HDK_random_images directory, then the line with OsvrHdkLedIdentifier_RANDOM_IMAGES_PATTERNS should be uncommented and the line with SENSOR0 commented out so that it looks for the correct patterns.  Note that it does not get a good pose with these images, but it can detect blobs and label some of them.
	VBHMD_DEBUG can be defined to create an openCV debugging video window for each sensor.  It also prints out the positions of all sensors.

Results: When running the test using commit c55fffc and debugging turned on by setting VBHMD_DEBUG, the Z position estimates were between 0.29991 and 0.30002 (expected 0.3).  The Y positions were between 0.02455 and 0.0248 (expected 0.025).  X varied as expected between -0.035 and 0.115.  All units are in meters.  So we've got less than 1mm of error with noise-free simulated images.

HDK_LED_patterns.txt:
	Patterns for the LEDs.  * means bright, . means dim, one LED encoded per line.  Each pattern is 16 bits.  LED1 is on the first line (the 0th LED).

HDK_LED_locations_3rmt.csv:
	Describes the spatial locations of the LEDs on the HDK in millimeters, from an origin about 50mm behind the center of the face plate.  The first 6 LEDs are located on the back side of the HDK, so have a different sensor number than the ones on the front.  The sensor number is ignored in the plug-in -- the sensor numbers are explicitly set with the front plate as 0 and back as 1 there, along with compiled-in patterns and locations.

read_beacon_info_into_blender.py:
	Blender Python script to read the above two files into Blender and produce LEDs that flash in the specified locations.

simulate_HDK_video.blend:
	Blender file to render the flashing LEDs from the point of view of a moving camera.  Can be played in a loop to get a viewpoint that oscillates.  It reads from the .csv and HDK*.txt files to determine where to place the LEDs.  It offsets the LEDs for sensors other than sensor 0 in Y so that they won't overlap with other sensors.

animation_from_fake:
	Directory containing the output rendering of simulate_HDK_video.blend.

