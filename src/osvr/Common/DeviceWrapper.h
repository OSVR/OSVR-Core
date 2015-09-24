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

#ifndef INCLUDED_DeviceWrapper_h_GUID_3524C649_2276_4C87_C116_663BABDC23C6
#define INCLUDED_DeviceWrapper_h_GUID_3524C649_2276_4C87_C116_663BABDC23C6

// Internal Includes
#include <osvr/Common/BaseDevice.h>
#include <osvr/Util/UniquePtr.h>

// Library/third-party includes
#include <vrpn_BaseClass.h>
#include <vrpn_ConnectionPtr.h>

// Standard includes
#include <string>

namespace osvr {
namespace common {
    /// @brief Base class used to implement BaseDevice/DeviceComponent-based
    /// devices on top of VRPN.
    class DeviceWrapper : public vrpn_BaseClass, public BaseDevice {
      public:
        DeviceWrapper(std::string const &name, vrpn_ConnectionPtr const &conn,
                      bool client);
        virtual ~DeviceWrapper();

      private:
        /// @name vrpn_BaseClass methods
        /// @{
        virtual void mainloop();
        virtual int register_types();
        /// @}
        /// @name BaseDevice methods
        /// @{
        virtual void m_update();
        /// @}
        vrpn_ConnectionPtr m_conn;
        bool m_client;
    };
} // namespace common
} // namespace osvr

#endif // INCLUDED_DeviceWrapper_h_GUID_3524C649_2276_4C87_C116_663BABDC23C6
