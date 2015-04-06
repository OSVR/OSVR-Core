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
#include <osvr/USBSerial/USBSerialEnum.h>
#include "USBSerialEnumImpl.h"

// Library/third-party includes

// Standard includes
#include <memory>
#include <iostream>

namespace osvr {
namespace usbserial {

    EnumeratorIterator::EnumeratorIterator(DeviceList *devices, int posn)
        : devs(devices), pos(posn){};

    EnumeratorIterator EnumeratorIterator::operator++() {
        ++pos;
        return *this;
    }
    USBSerialDevice *EnumeratorIterator::operator*() { return &devs->at(pos); }

    bool EnumeratorIterator::operator!=(const EnumeratorIterator &other) {
        return pos != other.pos;
    }

    EnumeratorIterator::~EnumeratorIterator() {}

    Enumerator::Enumerator(uint16_t vendorID, uint16_t productID)
        : m_impl(new EnumeratorImpl(vendorID, productID)){};

    Enumerator::~Enumerator() {}

    EnumeratorIterator Enumerator::begin() { return m_impl->begin(); }
    EnumeratorIterator Enumerator::end() { return (m_impl->end()); }

} // namespace usbserial
} // namespace osvr
