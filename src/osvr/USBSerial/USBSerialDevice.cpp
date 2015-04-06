/** @file
@brief Header

@date 2015

@author
Sensics, Inc.
<http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/USBSerial/USBSerialDevice.h>

// Library/third-party includes
// - none

// Standard includes
#include <memory>
#include <string>

namespace osvr {
namespace usbserial {

    // create USB-Serial device with provided vendor and product IDs
	USBSerialDevice::USBSerialDevice(uint16_t vendorID,
                                     uint16_t productID,
									 std::string devPath,
									 std::string port)
									 : deviceVID(vendorID), devicePID(productID),
									 devicePath(devPath), devicePort(port) {};

    USBSerialDevice::~USBSerialDevice() {}

	uint16_t USBSerialDevice::getVID() { return deviceVID; }
	uint16_t USBSerialDevice::getPID() { return devicePID; }
	std::string USBSerialDevice::getPlatformSpecificPath() { return devicePath; }
    std::string USBSerialDevice::getPort() { return devicePort; }

} // namespace usbserial
} // namespace osvr