# Measuring Camera/Video Stack Latency

With this new tracking core's filter implementation we're able to correctly fuse measurements from the IMU (which come in with very low latency) with measurements from the camera (which have higher latency, mostly imposed by the Windows UVC and kernel streaming stack used to access the camera), rewinding filter state and fusing/re-fusing in correct chronological order for a final result that is low jitter and low latency (and compensates for the camera latency so it is not reflected in the overall latency of the tracker).
This does depend on having an estimate of the "stimulus to frame" latency in the video stack for best performance, so video frames can be "backdated" from their capture timestamp to the assumed photon arrival time at the sensor.
The config file includes a value based on testing here with Windows 10 (1511) that should perform well in general, but building from source provides `uvbi-measure-camera-latency`, the tool that lets you measure this yourself in case you want to adjust it.

## Hardware setup

`uvbi-measure-camera-latency` requires one of the following:

- an Arduino (or other MCU) with an IR LED (and appropriate current-limiting resistor) that it turns on then off (pulse duration of 10 ms) upon receipt of every single byte over a 115200bps serial link (sketch included in subdirectory)

  - [Breadboard view of Arduino-based setup](IR cam latency tester using arduino_bb.png)

  - [Trivial schematic view of Arduino-based setup](IR cam latency tester using arduino_schem.png)

- a "Bus Pirate" prototyping device (v3 shown) with an IR LED and appropriate current-limiting resistor between the AUX pin and the GND pin.

  - [Breadboard view of Bus Pirate-based setup](IR cam latency tester using buspirate_bb.png)

  - [Trivial schematic view of Bus Pirate-based setup](IR cam latency tester using buspirate_schem.png)

(Diagrams created using Fritzing - source Fritzing files are in this folder)

## Operation and Usage

With either hardware setup, you hold the IR LED up to the IR camera and run the app, which will trigger the IR stimulus repeatedly (with a random delay in between) and measure the latency until a frame with a bright point (corresponding to the LED lighting up) arrives, using the same image capture backend as the tracker plugin.

These are the command line arguments for the tool - all optional with default values:

- `--bp`, `--buspirate`, or `-b`

  - use the buspirate serial "binary bit-bang raw" protocol - recommended, default

- `--arduino` or `-a`

  - use the arduino "single byte over serial" protocol

- `--dev` *`COM1`*

  - specify the device/port to use, here COM1 but of course you can substitute any serial port name, this is the default value

- `--rate` *`115200`*

  - specify the serial rate, not recommended to change this, this is the default value

- `--measurements` *`20`*

  - The number of stimuli to produce and measurements to take - 20 is the default value. Since the process is relatively quick, I've often used 100 instead.

It will:

- print the output to the console (several values per measurement cycle)
- save the "important" value for each measurement cycle to a CSV file named `latency.csv`
- and save a copy of the final "triggered" image to `triggered.png` for inspection to be sure your stimulus was working correctly and that you weren't getting false recognitions.

The value we're interested in is "Latency from trigger to sample time" (this is the one saved to CSV) - you'll want to use a value representative of the measurements taken, ignoring any suspicious measurements or obvious outliers.
The precise value isn't terribly important (at runtime, there is some variation anyway, especially on some Windows versions, as this tool will show) since a "close enough" value will often work, and it's not a tool we intend to ship in most cases or that we expect to be necessary for end users to use.

In the case of building a new/custom tracked object, it would be a useful feature to have the ability to similarly "trigger" an IR pulse with minimum latency, effectively building-in the functionality provided by the Bus Pirate or Arduino in this case.
The quickly-created app described here could be improved and integrated as a seamless part of a "first time setup" or "tuning" to really optimize performance on a user's machine.