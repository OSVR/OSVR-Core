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

#ifndef INCLUDED_MakeHDKTrackingSystem_h_GUID_B7C07A08_2BF8_45B2_4545_39979BA637E5
#define INCLUDED_MakeHDKTrackingSystem_h_GUID_B7C07A08_2BF8_45B2_4545_39979BA637E5

// Internal Includes
#include "TrackingSystem.h"
#include "TrackedBody.h"

#include "ConfigParams.h"

// Library/third-party includes
// - none

// Standard includes
#include <memory>

namespace osvr {
namespace vbtracker {

    inline std::unique_ptr<TrackingSystem>
    makeHDKTrackingSystem(ConfigParams const &params) {
        std::unique_ptr<TrackingSystem> sys;

        return sys;
    }
} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_MakeHDKTrackingSystem_h_GUID_B7C07A08_2BF8_45B2_4545_39979BA637E5
