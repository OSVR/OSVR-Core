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

// System includes
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOKitKeys.h>
#include <IOKit/serial/IOSerialKeys.h>
#include <IOKit/usb/IOUSBLib.h>

namespace osvr {
namespace usbserial {

    namespace {

        /**
         * Helper function that returns an IOKit iterator for all serial ports on the system.
         * @param matchingServices pointer which will be set to the value of the iterator
         * @return 0 on success, -1 on failure
         */
        int findSerialPorts(io_iterator_t *matchingServices) {

            kern_return_t kernResult;
            CFMutableDictionaryRef classesToMatch;
            // Query IOKit for services matching kIOSerialBSDServiceValue
            classesToMatch = IOServiceMatching(kIOSerialBSDServiceValue);
            if (classesToMatch == NULL) {
                return -1;
            }
            else {
                // Query only for serial ports
                CFDictionarySetValue(classesToMatch,
                                     CFSTR(kIOSerialBSDTypeKey),
                                     CFSTR(kIOSerialBSDAllTypes));
            }
            // Run the query
            kernResult = IOServiceGetMatchingServices(kIOMasterPortDefault, classesToMatch, matchingServices);
            if(KERN_SUCCESS !=kernResult) {
                return -1;
            }
            return 0;
        }
    }

    std::vector<USBSerialDevice>
    getSerialDeviceList(boost::optional<uint16_t> vendorID,
                        boost::optional<uint16_t> productID) {

        std::vector<USBSerialDevice> devices;

        io_iterator_t serialPortIterator;
        io_object_t serialPortService;
        // find all serial ports
        if(findSerialPorts(&serialPortIterator) != 0) {
            goto bailout;
        }

        // iterate over serial ports, getting vid and pid
        while((serialPortService = IOIteratorNext(serialPortIterator)) != 0) {
            const CFNumberRef vidObj = static_cast<CFNumberRef> (IORegistryEntrySearchCFProperty(serialPortService
                                                                                 , kIOServicePlane
                                                                                 , CFSTR("idVendor")
                                                                                 , NULL
                                                                                 , kIORegistryIterateRecursively | kIORegistryIterateParents));
            const CFNumberRef pidObj = static_cast<CFNumberRef> (IORegistryEntrySearchCFProperty(serialPortService
                                                                                 , kIOServicePlane
                                                                                 , CFSTR("idProduct")
                                                                                 , NULL
                                                                                 , kIORegistryIterateRecursively | kIORegistryIterateParents));
            const CFStringRef bsdPathObj = static_cast<CFStringRef> (IORegistryEntryCreateCFProperty(serialPortService,
                                                                          CFSTR(kIOCalloutDeviceKey),
                                                                          kCFAllocatorDefault,
                                                                          0));

            if(vidObj != NULL && pidObj != NULL && bsdPathObj != NULL) {
                // handle device
                uint16_t vid, pid;
                CFNumberGetValue(vidObj, kCFNumberSInt16Type, &vid);
                CFNumberGetValue(pidObj, kCFNumberSInt16Type, &pid);

                // printf("ALL DATA: VID: %d, PID: %d, PATH: %s\n", vid, pid, bsdPathBuf);
                if(vid == vendorID && pid == productID) {
                    // convert the string object into a C-string
                    CFIndex bufferSize = CFStringGetMaximumSizeForEncoding(CFStringGetLength(bsdPathObj), kCFStringEncodingMacRoman) + sizeof('\0');
                    std::vector<char> bsdPathBuf(bufferSize);
                    CFStringGetCString(bsdPathObj, &bsdPathBuf[0]
                        , bufferSize, kCFStringEncodingMacRoman);
                    // create the device
                    USBSerialDevice usb_serial_device(vid,
                                                      pid,
                                                      &bsdPathBuf[0], &bsdPathBuf[0]);
                    devices.push_back(usb_serial_device);
                }
            }
        }

    bailout:
        return devices;
    }

} // namespace usbserial
} // namespace osvr
