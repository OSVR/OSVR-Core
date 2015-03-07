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

#ifndef INCLUDED_DeviceTokenPtr_h_GUID_0B9B2AF5_09FE_4F8C_FDEC_98BE876F020A
#define INCLUDED_DeviceTokenPtr_h_GUID_0B9B2AF5_09FE_4F8C_FDEC_98BE876F020A

// Internal Includes
#include <osvr/Util/UniquePtr.h>

// Library/third-party includes
// - none

// Standard includes
// - none

struct OSVR_DeviceTokenObject;
namespace osvr {
namespace connection {
    typedef OSVR_DeviceTokenObject DeviceToken;
    typedef unique_ptr<DeviceToken> DeviceTokenPtr;
} // namespace connection
} // namespace osvr

#endif // INCLUDED_DeviceTokenPtr_h_GUID_0B9B2AF5_09FE_4F8C_FDEC_98BE876F020A
