/** @file
    @brief Header containing a typelist of all special report types.

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

#ifndef INCLUDED_ReportTypes_h_GUID_BB8F47AD_5F82_439E_C3F9_7C57D016D3C9
#define INCLUDED_ReportTypes_h_GUID_BB8F47AD_5F82_439E_C3F9_7C57D016D3C9

// Internal Includes
#include <osvr/Util/ClientCallbackTypesC.h>
#include <osvr/TypePack/List.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {
    /// @brief Type traits and metaprogramming supports related to
    /// specially-handled reports.
    namespace traits {
        /// @brief A typelist containing all internally-handled report types.
        using ReportTypeList = typepack::list<
            OSVR_AnalogReport, OSVR_ButtonReport, OSVR_PoseReport,
            OSVR_PositionReport, OSVR_OrientationReport, OSVR_VelocityReport,
            OSVR_LinearVelocityReport, OSVR_AngularVelocityReport,
            OSVR_AccelerationReport, OSVR_LinearAccelerationReport,
            OSVR_AngularAccelerationReport, OSVR_ImagingReport,
            OSVR_ImagingReport, OSVR_Location2DReport, OSVR_DirectionReport,
            OSVR_EyeTracker2DReport, OSVR_EyeTracker3DReport,
            OSVR_EyeTrackerBlinkReport, OSVR_NaviVelocityReport,
            OSVR_NaviPositionReport>;
    } // namespace traits

} // namespace common
} // namespace osvr

#endif // INCLUDED_ReportTypes_h_GUID_BB8F47AD_5F82_439E_C3F9_7C57D016D3C9
