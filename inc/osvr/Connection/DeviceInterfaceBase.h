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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_DeviceInterfaceBase_h_GUID_F804FF7E_EF77_4280_8597_CFFC18D53DFD
#define INCLUDED_DeviceInterfaceBase_h_GUID_F804FF7E_EF77_4280_8597_CFFC18D53DFD

// Internal Includes
#include <osvr/Connection/DeviceToken.h>
#include <osvr/Util/GuardPtr.h>

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
// - none

namespace osvr {
namespace connection {
    /// @brief Base class for the DeviceInterfaceObjects retrieved by
    /// plugins to let them send data on an interface. Contains
    /// functionality shared between all such interface objects.
    class DeviceInterfaceBase {
      public:
        void setDeviceToken(DeviceToken &dev) { m_token = &dev; }
        util::GuardPtr getSendGuard() {
            BOOST_ASSERT_MSG(m_token != nullptr, "Can't get a send guard "
                                                 "before we've been supplied "
                                                 "with a device token!");
            return m_token->getSendGuard();
        }

      private:
        DeviceToken *m_token = nullptr;
    };
} // namespace connection
} // namespace osvr

#endif // INCLUDED_DeviceInterfaceBase_h_GUID_F804FF7E_EF77_4280_8597_CFFC18D53DFD
