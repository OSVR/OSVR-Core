# OSVR List USB Serial              {#OSVRListUSBSerial}

This is a minimal tool wrapping the @ref osvr::USBSerial library that OSVR-Core provides for plugin developers, to make it easy to write plugins for devices that interact over a USB serial port. Similarly, this tool is most likely to be useful to developers.

It takes no parameters, and runs on the command line. On a sample Windows system with an OSVR HDK plugged in, it output the following:

~~~
> osvr_list_usbserial.exe
1532:0b00 \\.\COM5
~~~

This shows I had one USB serial device plugged in that was detected. It has the vendor ID, in hex, of 1532, and the product ID, in hex, of 0b00. (This corresponds to the OSVR HDK.)  Next to that, you see the "platform-specific path" to the serial port. On Windows, the way to be able to access all serial ports, even high-numbered ones, is to prefix them with `\\.\` - so you can see the library reported my HDK is accessible at `COM5` right now.

In addition to what the command line program lets you do, the corresponding library lets you enumerate USB serial ports pre-filtering by vendor and product ID, so you can easily write a hardware-detection method for your device.
