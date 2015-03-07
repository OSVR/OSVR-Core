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

#ifndef INCLUDED_GenerateVrpnDynamicServer_h_GUID_6828B8BC_E58A_4DAE_E511_46B854C51C11
#define INCLUDED_GenerateVrpnDynamicServer_h_GUID_6828B8BC_E58A_4DAE_E511_46B854C51C11

// Internal Includes
#include "DeviceConstructionData.h"

// Library/third-party includes
#include <vrpn_MainloopObject.h>

// Standard includes
// - none

namespace osvr {
namespace connection {

    namespace server_generation {
        /// @brief the type we'll pass to each constructor
        typedef DeviceConstructionData ConstructorArgument;
    } // namespace server_generation
    vrpn_MainloopObject *
    generateVrpnDynamicServer(server_generation::ConstructorArgument &init);
} // namespace connection
} // namespace osvr

#endif // INCLUDED_GenerateVrpnDynamicServer_h_GUID_6828B8BC_E58A_4DAE_E511_46B854C51C11
