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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_DeviceComponentPtr_h_GUID_D2258125_987B_42A9_7215_3EED5B9622BB
#define INCLUDED_DeviceComponentPtr_h_GUID_D2258125_987B_42A9_7215_3EED5B9622BB

// Internal Includes
#include <osvr/Util/SharedPtr.h>

// Library/third-party includes
// - none

// Standard includes
#include <vector>

namespace osvr {
namespace common {
    class DeviceComponent;
    typedef shared_ptr<DeviceComponent> DeviceComponentPtr;
    typedef std::vector<DeviceComponentPtr> DeviceComponentList;
} // namespace common
} // namespace osvr

#endif // INCLUDED_DeviceComponentPtr_h_GUID_D2258125_987B_42A9_7215_3EED5B9622BB
