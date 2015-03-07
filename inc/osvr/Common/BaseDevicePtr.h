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

#ifndef INCLUDED_BaseDevicePtr_h_GUID_F97A9FAF_5861_4C97_D38C_A223C26D61E6
#define INCLUDED_BaseDevicePtr_h_GUID_F97A9FAF_5861_4C97_D38C_A223C26D61E6

// Internal Includes
#include <osvr/Util/SharedPtr.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {
    class BaseDevice;
    typedef shared_ptr<BaseDevice> BaseDevicePtr;
} // namespace common
} // namespace osvr

#endif // INCLUDED_BaseDevicePtr_h_GUID_F97A9FAF_5861_4C97_D38C_A223C26D61E6
