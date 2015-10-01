/** @file
    @brief Implementation

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
#include "USBSerialDevInfo.h"

// Library/third-party includes
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string.hpp>

// Standard includes
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include <fcntl.h>        // for O_NONBLOCK
#include <sys/ioctl.h>    // for ioctl
#include <unistd.h>       // for open, close

// IOKit includes
#include <IOKit/IOKitLib.h>
#include <IOKit/usb/IOUSBLib.h>

namespace osvr {
namespace usbserial {

    namespace {

        /**
         * Given the name of a device, this function will create a
         *USBSerialDevice
         * object. If the device can be created successfully it will return
         * @c boost::none.
         *
         * @param device The name of the device to create (e.g., "ttyACM0")
         *
         * @return an optional USBSerialDevice
         */
        boost::optional<USBSerialDevice>
        make_USBSerialDevice(const std::string &device) {
            //FIXME: STUB
            return boost::none;
        }

        /**
         * Predicate that determines if the given vendor and product IDs match
         * that of the provided USB serial device. If the @param vendorID or
         * @param productID is @c boost::none, that parameter will match and ID.
         *
         * @param device the USB serial device to compare against
         * @param vendorID optional vendor ID
         * @param productID optional product ID
         *
         * @return true if the vendor and product IDs match, false otherwise
         */
        bool matches_ids(const USBSerialDevice &device,
                         const boost::optional<uint16_t> &vendorID,
                         const boost::optional<uint16_t> &productID) {
            const bool vendor_matches =
                (!vendorID || *vendorID == device.getVID());
            const bool product_matches =
                (!productID || *productID == device.getPID());

            return (vendor_matches && product_matches);
        }

    } // end namespace

    std::vector<USBSerialDevice>
    getSerialDeviceList(boost::optional<uint16_t> vendorID,
                        boost::optional<uint16_t> productID) {

        //FIXME: STUB
        std::vector<USBSerialDevice> devices;
        return devices;
    }

} // namespace usbserial
} // namespace osvr
