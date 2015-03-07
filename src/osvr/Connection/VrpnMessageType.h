/** @file
    @brief Header

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
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

#ifndef INCLUDED_VrpnMessageType_h_GUID_ECDE1AD9_A386_4EA3_CA02_C5D95B16B2D1
#define INCLUDED_VrpnMessageType_h_GUID_ECDE1AD9_A386_4EA3_CA02_C5D95B16B2D1

// Internal Includes
#include <osvr/Connection/MessageType.h>

// Library/third-party includes
#include <vrpn_ConnectionPtr.h>

// Standard includes
// - none

namespace osvr {
namespace connection {

    class VrpnMessageType : public MessageType {
      public:
        VrpnMessageType(std::string const &name, vrpn_ConnectionPtr &vrpnConn)
            : MessageType(name) {
            m_messageID = vrpnConn->register_message_type(name.c_str());
        }
        ~VrpnMessageType() {}

        vrpn_int32 getID() const { return m_messageID; }

      private:
        vrpn_int32 m_messageID;
    };
} // namespace connection
} // namespace osvr

#endif // INCLUDED_VrpnMessageType_h_GUID_ECDE1AD9_A386_4EA3_CA02_C5D95B16B2D1
