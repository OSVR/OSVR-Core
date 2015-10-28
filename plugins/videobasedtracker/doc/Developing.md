# Developing for the OSVR video-based tracker

This document describes how to develop new devices that will work with the OSVR video-based tracker, including how to design the flash patterns that should be used.  See the [running document](./Running.md) for how to use the camera to track.


## Brief theory of operation

A *beacon* is light source that emits in the near infrared (an LED, for example).  A *sensor* is a device that has four or more beacons rigidly mounted on its surface.

The OSVR video-based tracking system tracks one or more sensors.  The beacons on each sensor flash in synchronization with the video camera, controlled by a digital signal the comes from the camera itself.  On the OSVR HDK, this signal plugs in using a Y adapter to the power cord (as shown below).

![Synchronized power plug](./sync_power_adapter.png)

The camera takes images at about 100 frames/second.  Each beacon strobes briefly during this time, with the brightness of each consecutive strobe modulated to match a 16-frame pattern.  The exact pattern is detailed below, but each beacon basically has two possible brightnesses for each frame, which we'll call *dim* and *bright*.  Each beacon has a unique 16-frame pattern, which is used to distinguish it from other beacons.  (Note that lights, other LEDs in the environment, and the sun will produce spots on the image but they will not strobe with a specific pattern, so are not recognized as beacons by the tracking system.)

The video-based tracking system is provided with a description for each sensor that includes the strobe patterns of its beacons and also the 3D positions where they are mounted.  It also has a model of the camera's distortion.  It uses the flashing pattern to identify the 2D screen-space positions of each beacon that is visible.  It then finds an error-minimizing pose in 3-space for those beacons to minimize the error of the 3D positions projected through the camera model back onto the screen.  When enough beacons are seen and the error is small enough, it produces a report telling the pose of the sensor with respect to the camera's center of projection and orientation.

## New sensor design

If you want to make a new sensor that is compatible with the OSVR video-based tracking system, it needs to have beacon flash patterns and layouts that are consistent with the system and which do not include beacons whose flash patterns match any other beacons in the system, in particular the exiting 40 LEDs on the front and back panels of the OSVR HDK.

### Flash characteristics

### Flash patterns

XXX Rotationally invariant.
XXX Not conflicting with existing patterns.  XXX Find the available ones in odd parity.  XXX Even parity ones are available for other sensors.

XXX Include the program to find patterns.

### Beacon layout

XXX Coordinate system.
XXX separated enough.
XXX 4+ beacons always visible.
XXX Non-degenerate positions.

### Config files

## Appendix: Design criteria and resulting coding for OSVR HDK

The assumptions on the system characteristics, which must be satisfied for the proposed design to work, are given first.  Next come the criteria being optimized, from most important to least.  Different prioritizations for criteria result in different optimal coding algorithms (for example, it is possible to achieve a single bright LED at any given time for a non-error-detecting code using information across LEDs by making a code that is 50 frames long, but this would require almost a second to determine LED identity for a camera operating at 60fps and would require seeing all LEDs).

### Assumptions
* The LED flashing will be synchronized with the camera frames such that they are on high-power or low-power for the duration of exposure for consecutive frames.  This will allow the computer-vision algorithm to reliably detect data values at full frame rate.
* The brightness difference between high power and low power is sufficient to be reliably detected, removing the need for error correcting codes.

### Criteria

* Make a code that is independent, so that each LED can be treated on its own.  **Solution:** Rotationally-invariant code selected.
* Minimize the maximum instantaneous power draw over all LEDs.  **Approach:** A set of encodings with the minimum number of 1’s, subject to being rotationally invariant, was selected.  An iterative optimization algorithm was run to select the best set of rotations, which matched the theoretical minimum number of simultaneous LEDs.
* Minimize time to determine LED identities for 40 LEDs.  **Approach:** A subset of the binary code is selected, where the binary code is the most-efficient encoding of bits.
* Provide an error check so that single-bit errors can be detected.  **Approach:** The set of encodings selected for the HDK has odd parity, so any single-bit error will appear as an invalid code.

### Design

Each LED pattern consists of an infinitely-repeated series of frames.  Each frame consists of a series of bright(1)/dim(0) bits lasting for one camera frame, synchronized with the camera exposures.

There are N bits within a frame (N=16 was selected for reasons described below).  See a later secion for LED pattern tables.  The patterns have the characteristic that no patterns can be rotated to produce another so that the ID for a particular LED can be reliably determined by looking only at that LED’s sequence.  The relative start time of the transmissions for different LEDs does not matter.  Patterns with even, odd, and no parity checking were tested, to see the impact of enabling parity checking to detect single-bit errors.

This forms a family of encodings parameterized by N (the number of bits used to encode) and parity.  As N increases, the time to determine a complete encoding increases but the maximum number of overlapping LEDs in the high state decreases.  The minimum counts with optimal packing was computed for a range of choices and a size of 16 with odd parity was selected.

#### Encoding

The encoding selected for the OSVR HDK is shown below.  Each period ('.') indicates a dim flash nd each asterisk ('*') indicates a bright flash.  The LED index is listed along the left edge (this does not match the final hardware ordering in the as-designed unit).

     0: ***...*........*
     1: ...****..*......
     2: *.*..........***
     3: **...........***
     4: *....*....*.....
     5: ...*....*...*...
     6: ..*.....*...*...
     7: ...*......*...*.
     8: .......*...*...*
     9: ......*...*..*..
    10: .......*....*..*
    11: ..*.....*..*....
    12: ....*......*..*.
    13: ....*..*....*...
    14: ..*...*........*
    15: ........*..*..*.
    16: ..*..*.*........
    17: ....*...*.*.....
    18: ...*.*........*.
    19: ...*.....*.*....
    20: ....*.*......*..
    21: *.......*.*.....
    22: .*........*.*...
    23: .*.........*.*..
    24: ....*.*..*......
    25: .*.*.*..........
    26: .........*.**...
    27: **...........*..
    28: .*...**.........
    29: .........*....**
    30: ..*.....**......
    31: *......**.......
    32: ...*.......**...
    33: ...**.....*.....
    34: .**....*........
    35: ....**...*......
    36: *...........**..
    37: ......**.*......
    38: .............***
    39: ..........*.....

To minimize the maximum instantaneous power, the patterns have been optimally rotated.  A count of the number of LEDs on per time step is: 8 8 8 8 8 7 8 7 8 8 8 8 8 8 8 8.

