/** @file
    @brief Header

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
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

#ifndef INCLUDED_BodyTargetInterface_h_GUID_FA3B619A_D69E_45BC_DF03_CDF4AEFF542E
#define INCLUDED_BodyTargetInterface_h_GUID_FA3B619A_D69E_45BC_DF03_CDF4AEFF542E

// Internal Includes
#include "ModelTypes.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace vbtracker {

    /// A way for targets to access internals of a tracked body.
    struct BodyTargetInterface {
        BodyTargetInterface(BodyState &state_) : state(state_) {}
        BodyState &state;
    };

} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_BodyTargetInterface_h_GUID_FA3B619A_D69E_45BC_DF03_CDF4AEFF542E
