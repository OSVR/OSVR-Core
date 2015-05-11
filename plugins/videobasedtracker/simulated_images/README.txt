This directory contains information needed to simulate images from the OSVR HDK, using the original designed LED locations and flash patterns.

Author: Sensics.com
License: Apache 2.0

Usage: VBHMD_FAKE_IMAGES is defined in ../com_osvr_VideoBAsedHMDTracker.cpp and pointed at a directory containing the images simulated below, it will use those in a loop to test the tracking alorithm.

Results: When running the test using commit c55fffc and debugging turned on by setting VBHMD_DEBUG, the Z position estimates were between 0.29991 and 0.30002 (expected 0.3).  The Y positions were between 0.02455 and 0.0248 (expected 0.025).  X varied as expected between -0.035 and 0.115.  All units are in meters.  So we've got less than 1mm of error with noise-free simulated images.

HDK_LED_patterns.txt:
	Patterns for the LEDs.  * means bright, . means dim, one LED encoded per line.  Each pattern is 16 bits.  LED1 is on the first line (the 0th LED).

HDK_LED_locations_2rmt.csv:
	Describes the spatial locations of the LEDs on the HDK in millimeters, from an origin about 50mm behind the center of the face plate.  The first 6 LEDs are located where they cannot be seen, so are indicated with Z=-10000 in the file.

read_beacon_info_into_blender.py:
	Blender Python script to read the above two files into Blender and produce LEDs that flash in the specified locations.

simulate_HDK_video.blend:
	Blender file to render the flashing LEDs from the point of view of a moving camera.  Can be played in a loop to get a viewpoint that oscillates.

animation_from_fake:
	Directory containing the output rendering of simulate_HDK_video.blend.


