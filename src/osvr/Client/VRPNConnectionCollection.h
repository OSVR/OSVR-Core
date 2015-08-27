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

#ifndef INCLUDED_VRPNConnectionCollection_h_GUID_AB4B5310_43DF_4E9A_2EAF_CF512F979288
#define INCLUDED_VRPNConnectionCollection_h_GUID_AB4B5310_43DF_4E9A_2EAF_CF512F979288

// Internal Includes
#include <osvr/Util/SharedPtr.h>
#include <osvr/Common/PathElementTypes.h>
#include <osvr/Client/Export.h>

// Library/third-party includes
#include <vrpn_ConnectionPtr.h>

// Standard includes
#include <string>
#include <unordered_map>

namespace osvr {
namespace client {
    class VRPNConnectionCollection {
      public:
        OSVR_CLIENT_EXPORT VRPNConnectionCollection();

        OSVR_CLIENT_EXPORT vrpn_ConnectionPtr
        addConnection(vrpn_ConnectionPtr conn, std::string const &host);

        vrpn_ConnectionPtr getConnection(std::string const &device,
                                         std::string const &host);
        vrpn_ConnectionPtr
        getConnection(common::elements::DeviceElement const &elt);
        OSVR_CLIENT_EXPORT void updateAll();

      private:
        typedef std::unordered_map<std::string, vrpn_ConnectionPtr>
            ConnectionMap;
        shared_ptr<ConnectionMap> m_connMap;
    };

} // namespace client
} // namespace osvr

#endif // INCLUDED_VRPNConnectionCollection_h_GUID_AB4B5310_43DF_4E9A_2EAF_CF512F979288
