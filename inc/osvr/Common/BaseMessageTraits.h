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

#ifndef INCLUDED_BaseMessageTraits_h_GUID_AB3AFAC5_54F4_41BF_78D9_CE10525DD053
#define INCLUDED_BaseMessageTraits_h_GUID_AB3AFAC5_54F4_41BF_78D9_CE10525DD053

// Internal Includes
#include <osvr/Common/BaseDevice.h>
#include <osvr/Common/MessageHandler.h>

// Library/third-party includes
#include <vrpn_BaseClass.h>

// Standard includes
// - none

namespace osvr {
namespace common {
    /// @brief Traits class for use with MessageHandler.
    typedef ImpliedSenderMessageHandleTraits<vrpn_MESSAGEHANDLER, BaseDevice>
        BaseDeviceMessageHandleTraits;
} // namespace common
} // namespace osvr
#endif // INCLUDED_BaseMessageTraits_h_GUID_AB3AFAC5_54F4_41BF_78D9_CE10525DD053
