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

#ifndef INCLUDED_USBSerialEnum_h_GUID_74DF3866_AA0B_4FB9_05D5_5FC824D366DC
#define INCLUDED_USBSerialEnum_h_GUID_74DF3866_AA0B_4FB9_05D5_5FC824D366DC

// Internal Includes
#include <osvr/USBSerial/Export.h>
#include <osvr/USBSerial/USBSerialDevice.h>

// Library/third-party includes
// - none

// Standard includes
#include <memory>
#include <iostream>
#include <vector>

namespace osvr {
namespace usbserial {

    typedef std::vector<USBSerialDevice> DeviceList;

    class EnumeratorImpl;

    class EnumeratorIterator {

      public:
        OSVR_USBSERIAL_EXPORT EnumeratorIterator(DeviceList const &devs,
                                                 std::size_t posn);
        OSVR_USBSERIAL_EXPORT EnumeratorIterator &operator++();
        OSVR_USBSERIAL_EXPORT USBSerialDevice const &operator*() const;
        OSVR_USBSERIAL_EXPORT bool
        operator!=(const EnumeratorIterator &other) const;

      private:
        DeviceList const *devs = nullptr;
        std::size_t pos;
    };
    class Enumerator {
        class Impl;

        OSVR_USBSERIAL_EXPORT Enumerator(uint16_t vID, uint16_t pID);
        OSVR_USBSERIAL_EXPORT Enumerator();
        friend Enumerator enumerate();
        friend Enumerator enumerate(uint16_t vID, uint16_t pID);

      public:
        Enumerator(Enumerator const &) = delete;
        Enumerator &operator=(Enumerator const &) = delete;

        /// move constructor
        OSVR_USBSERIAL_EXPORT Enumerator(Enumerator &&other);
        OSVR_USBSERIAL_EXPORT ~Enumerator();

        OSVR_USBSERIAL_EXPORT EnumeratorIterator begin();
        OSVR_USBSERIAL_EXPORT EnumeratorIterator end();

      private:
        std::unique_ptr<EnumeratorImpl> m_impl;
    };

    inline Enumerator enumerate() { return Enumerator{}; }
    inline Enumerator enumerate(uint16_t vID, uint16_t pID) {
        return Enumerator{vID, pID};
    }

} // namespace usbserial
} // namespace osvr
#endif // INCLUDED_USBSerialEnum_h_GUID_74DF3866_AA0B_4FB9_05D5_5FC824D366DC
