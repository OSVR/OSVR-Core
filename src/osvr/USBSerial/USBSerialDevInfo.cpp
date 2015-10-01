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
#include <osvr/Util/PlatformConfig.h>

// Library/third-party includes
// - none

// Standard includes
#include <string>

#if defined(OSVR_WINDOWS)
#include "USBSerialDevInfo_Windows.h"
#elif defined(OSVR_LINUX)
#include "USBSerialDevInfo_Linux.h"
#elif defined(OSVR_ANDROID)
#include "USBSerialDevInfo_Linux.h"
#elif defined(OSVR_MACOSX)
#include "USBSerialDevInfo_MacOSX.h"
#else
#error "getPortNumber() not yet implemented for this platform!"
namespace osvr {
namespace usbserial {

    std::string getPortNumber(USBSerialDeviceImpl *serialDevice) {}

} // namespace usbserial
} // namespace osvr
#endif
