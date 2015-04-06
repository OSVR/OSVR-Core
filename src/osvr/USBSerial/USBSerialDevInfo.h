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

#ifndef INCLUDED_SerialPort_h_GUID_DC91C7DB_E233_4C95_03F7_31415985F7B8
#define INCLUDED_SerialPort_h_GUID_DC91C7DB_E233_4C95_03F7_31415985F7B8

// Internal Includes
#include <osvr/USBSerial/USBSerialDevice.h>

// Library/third-party includes

// Standard includes
#include <string>
#include <vector>

namespace osvr {
namespace usbserial {

	std::vector<USBSerialDevice> getSerialDeviceList(uint16_t vendorID, uint16_t productID);

} // namespace usbserial
} // namespace osvr

#endif // INCLUDED_SerialPort_h_GUID_DC91C7DB_E233_4C95_03F7_31415985F7B8
