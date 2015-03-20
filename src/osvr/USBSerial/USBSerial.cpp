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
#include <osvr/USBSerial/USBSerial.h>
#include "USBSerialDeviceImpl.h"

// Library/third-party includes
// - none

// Standard includes
#include <memory>
#include <string>

namespace osvr {
namespace usbserial {

    // create USB-Serial device with provided vendor and product IDs
    USBSerialDevice::USBSerialDevice(std::string vendorID,
                                     std::string productID)
        : impl(new USBSerialDeviceImpl(vendorID, productID)){};

    USBSerialDevice::~USBSerialDevice() {}

    std::string USBSerialDevice::getVID() { return impl->getVID(); }

    std::string USBSerialDevice::getPID() { return impl->getPID(); }

    std::string USBSerialDevice::getPort() { return impl->getPort(); }

} // namespace usbserial
} // namespace osvr