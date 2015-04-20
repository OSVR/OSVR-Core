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

#ifndef INCLUDED_USBSerialEnum_h_GUID_B17561E7_ADC2_438C_87B9_D05321AF6BD6
#define INCLUDED_USBSerialEnum_h_GUID_B17561E7_ADC2_438C_87B9_D05321AF6BD6

// Internal Includes
#include <osvr/USBSerial/Export.h>
#include <osvr/Util/StdInt.h>

// Library/third-party includes
// - none

// Standard includes
#include <string>

namespace osvr {
namespace usbserial {

    class USBSerialDevice {
      public:
        OSVR_USBSERIAL_EXPORT USBSerialDevice(uint16_t vendorID,
                                              uint16_t productID,
                                              const std::string &devPath,
                                              const std::string &port);
        OSVR_USBSERIAL_EXPORT USBSerialDevice();

        OSVR_USBSERIAL_EXPORT uint16_t getVID() const;
        OSVR_USBSERIAL_EXPORT uint16_t getPID() const;
        OSVR_USBSERIAL_EXPORT std::string getPlatformSpecificPath() const;
        OSVR_USBSERIAL_EXPORT std::string getPort() const;

      private:
        uint16_t m_vendorID;
        uint16_t m_productID;
        std::string m_devicePath;
        std::string m_devicePort;
    };

    inline USBSerialDevice::USBSerialDevice(uint16_t vendorID,
                                            uint16_t productID,
                                            const std::string &devPath,
                                            const std::string &port)
        : m_vendorID(vendorID), m_productID(productID), m_devicePath(devPath),
          m_devicePort(port) {
        // do nothing
    }

    inline USBSerialDevice::USBSerialDevice() {
        // do nothing
    }

    inline uint16_t USBSerialDevice::getVID() const { return m_vendorID; }

    inline uint16_t USBSerialDevice::getPID() const { return m_productID; }

    inline std::string USBSerialDevice::getPlatformSpecificPath() const {
        return m_devicePath;
    }

    inline std::string USBSerialDevice::getPort() const { return m_devicePort; }

} // namespace usbserial
} // namespace osvr

#endif // INCLUDED_USBSerialEnum_h_GUID_B17561E7_ADC2_438C_87B9_D05321AF6BD6
