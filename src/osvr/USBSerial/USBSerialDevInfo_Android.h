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
#include "USBSerial_Android.h"

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
            // Vendor ID stored in file /sys/class/tty/DEVICE/device/uevent
            //
            //    PRODUCT=2341/1/1
            //
            // Vendor ID / Product ID / who cares?
            //
            // Values are in hexadecimal

            boost::filesystem::path uevent_file =
                "/sys/class/tty/" + device + "/device/uevent";
            if (!boost::filesystem::exists(uevent_file)) {
                return boost::none;
            }

            std::ifstream uevent_input(uevent_file.generic_string());
            std::string line;
            while (std::getline(uevent_input, line)) {
                if (!boost::starts_with(line, "PRODUCT="))
                    continue;

                // Extract the vendor and product IDs
                const auto first = line.find("=");
                const auto second = line.find("/", first);
                const auto third = line.find("/", second + 1);

                const std::string vendor_id_str =
                    line.substr(first + 1, second - first - 1);
                const std::string product_id_str =
                    line.substr(second + 1, third - second - 1);

                const uint16_t detected_vendor_id =
                    std::stoi(vendor_id_str.c_str(), 0, 16);
                const uint16_t detected_product_id =
                    std::stoi(product_id_str.c_str(), 0, 16);

                // Construct the USBSerialDevice
                const std::string device_name =
                    (boost::filesystem::path("/dev") / device).generic_string();
                USBSerialDevice usb_serial_device(detected_vendor_id,
                                                  detected_product_id,
                                                  device_name, device_name);
                return usb_serial_device;
            }

            return boost::none;
        }

        /**
         * Probe the serial port to see if a device is actually connected.
         *
         * @param port The device name to probe (e.g., "/dev/ttyACM0")
         *
         * @return true if a device is connected, false otherwise
         */
        bool serial8250_device_connected(const std::string &port) {
            const int fd = open(port.c_str(), O_RDWR | O_NONBLOCK | O_NOCTTY);
            if (-1 == fd) {
                return false;
            }

            struct serial_struct serial_info;
            const int ret = ioctl(fd, TIOCGSERIAL, &serial_info);
            if (-1 == ret) {
                close(fd);
                return false;
            }

            if (PORT_UNKNOWN == serial_info.type) {
                close(fd);
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

    } // end namespace

    std::vector<USBSerialDevice>
    getSerialDeviceList(boost::optional<uint16_t> vendorID,
                        boost::optional<uint16_t> productID) {

        std::vector<USBSerialDevice> devices;

        // Get a list of all TTY devices in /sys/class/tty
        const boost::filesystem::path sys_class_tty = "/sys/class/tty";
        for (const auto &path_name : boost::make_iterator_range(
                 boost::filesystem::directory_iterator(sys_class_tty),
                 boost::filesystem::directory_iterator())) {
            // Filter out entries that don't have a .../device/driver file
            const boost::filesystem::path device_candidate = path_name;
            const boost::filesystem::path driver_file =
                device_candidate / "device" / "driver";
            if (!boost::filesystem::exists(driver_file)) {
                continue;
            }

            // Device name is /dev/ttySomething
            const boost::filesystem::path basename =
                boost::filesystem::basename(device_candidate);

            auto usb_serial_device =
                make_USBSerialDevice(basename.generic_string());
            if (!usb_serial_device)
                continue;

            // Check to see if it matches the vendor and product ID
            if (!matches_ids(*usb_serial_device, vendorID, productID))
                continue;

            // If the driver is serial8250, check to see if a device is actually
            // connected to the port
            if (boost::filesystem::exists(driver_file / "serial8250")) {
                const bool is_connected = serial8250_device_connected(
                    "/dev/" + basename.generic_string());
                if (!is_connected) {
                    continue;
                }
            }

            devices.push_back(*usb_serial_device);
        }

        return devices;
    }

} // namespace usbserial
} // namespace osvr
