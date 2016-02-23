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

// System includes
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOKitKeys.h>
#include <IOKit/serial/IOSerialKeys.h>

namespace osvr {
namespace usbserial {

    namespace {

        /**
         * Helper function that returns an IOKit iterator for all serial ports
         * on the system.
         * @param matchingServices pointer which will be set to the value of the
         * iterator
         * @return 0 on success, -1 on failure
         */
        int findSerialPorts(io_iterator_t *matchingServices) {
            // Query IOKit for services matching kIOSerialBSDServiceValue
            CFMutableDictionaryRef classesToMatch =
                IOServiceMatching(kIOSerialBSDServiceValue);
            if (classesToMatch == NULL) {
                return false;
            }
            // Query only for serial ports
            CFDictionarySetValue(classesToMatch, CFSTR(kIOSerialBSDTypeKey),
                                 CFSTR(kIOSerialBSDAllTypes));
            // Run the query
            kern_return_t kernResult = IOServiceGetMatchingServices(
                kIOMasterPortDefault, classesToMatch, matchingServices);
            if (KERN_SUCCESS != kernResult) {
                return false;
            }
            return true;
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
    }

    std::vector<USBSerialDevice>
    getSerialDeviceList(boost::optional<uint16_t> vendorID,
                        boost::optional<uint16_t> productID) {

        std::vector<USBSerialDevice> devices;

        io_iterator_t serialPortIterator;
        io_object_t serialPortService;
        // find all serial ports
        if (findSerialPorts(&serialPortIterator) == true) {
            // iterate over serial ports, getting vid and pid
            while ((serialPortService = IOIteratorNext(serialPortIterator)) !=
                   0) {
                const CFNumberRef vidObj =
                    static_cast<CFNumberRef>(IORegistryEntrySearchCFProperty(
                        serialPortService, kIOServicePlane, CFSTR("idVendor"),
                        NULL, kIORegistryIterateRecursively |
                                  kIORegistryIterateParents));
                const CFNumberRef pidObj =
                    static_cast<CFNumberRef>(IORegistryEntrySearchCFProperty(
                        serialPortService, kIOServicePlane, CFSTR("idProduct"),
                        NULL, kIORegistryIterateRecursively |
                                  kIORegistryIterateParents));
                const CFStringRef bsdPathObj =
                    static_cast<CFStringRef>(IORegistryEntryCreateCFProperty(
                        serialPortService, CFSTR(kIOCalloutDeviceKey),
                        kCFAllocatorDefault, 0));

                if (!vidObj || !pidObj || !bsdPathObj) {
                    continue;
                }
                // handle device
                uint16_t vid, pid;
                CFNumberGetValue(vidObj, kCFNumberSInt16Type, &vid);
                CFNumberGetValue(pidObj, kCFNumberSInt16Type, &pid);

                // convert the string object into a C-string
                CFIndex bufferSize = CFStringGetMaximumSizeForEncoding(
                                         CFStringGetLength(bsdPathObj),
                                         kCFStringEncodingMacRoman) +
                                     sizeof('\0');
                std::vector<char> bsdPathBuf(bufferSize);
                CFStringGetCString(bsdPathObj, bsdPathBuf.data(), bufferSize,
                                   kCFStringEncodingMacRoman);
                // create the device
                USBSerialDevice usb_serial_device(vid, pid, bsdPathBuf.data(),
                                                  bsdPathBuf.data());
                // check if IDs match and add
                if (matches_ids(usb_serial_device, vendorID, productID)) {
                    devices.push_back(usb_serial_device);
                }
            }
        }

        return devices;
    }

} // namespace usbserial
} // namespace osvr
