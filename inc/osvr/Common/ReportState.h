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

#ifndef INCLUDED_ReportState_h_GUID_452FA875_E51C_4A88_5A58_E28CB6F40B86
#define INCLUDED_ReportState_h_GUID_452FA875_E51C_4A88_5A58_E28CB6F40B86

// Internal Includes
#include <osvr/Util/ClientCallbackTypesC.h>
#include <osvr/Common/StateType.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {
    namespace traits {

        /// @brief Helper traits struct for reportState(), to access the state
        /// member of the various report types.
        ///
        /// Default implementation is fine for any reports where the state
        /// member of the report struct is called simply `state` - only
        /// explicitly specialize if that isn't the case.
        template <typename ReportType> struct ReportStateGetter {
            static typename StateType<ReportType>::type const &
            apply(ReportType const &r) {
                return r.state;
            }
            static typename StateType<ReportType>::type apply(ReportType &r) {
                return r.state;
            }
        };

        // Template specialization to handle OSVR_PositionReport
        template <> struct ReportStateGetter<OSVR_PositionReport> {
            static OSVR_PositionState const &
            apply(OSVR_PositionReport const &r) {
                return r.xyz;
            }
            static OSVR_PositionState apply(OSVR_PositionReport &r) {
                return r.xyz;
            }
        };

        // Template specialization to handle OSVR_PoseReport
        template <> struct ReportStateGetter<OSVR_PoseReport> {
            static OSVR_PoseState const &apply(OSVR_PoseReport const &r) {
                return r.pose;
            }
            static OSVR_PoseState apply(OSVR_PoseReport &r) { return r.pose; }
        };

        // Template specialization to handle OSVR_OrientationReport
        template <> struct ReportStateGetter<OSVR_OrientationReport> {
            static OSVR_OrientationState const &
            apply(OSVR_OrientationReport const &r) {
                return r.rotation;
            }
            static OSVR_OrientationState apply(OSVR_OrientationReport &r) {
                return r.rotation;
            }
        };

        // Template specialization to handle OSVR_Location2DReport
        template <> struct ReportStateGetter<OSVR_Location2DReport> {
            static OSVR_Location2DState const &
            apply(OSVR_Location2DReport const &r) {
                return r.location;
            }
            static OSVR_Location2DState apply(OSVR_Location2DReport &r) {
                return r.location;
            }
        };

        // Template specialization to handle OSVR_DirectionReport
        template <> struct ReportStateGetter<OSVR_DirectionReport> {
            static OSVR_DirectionState const &
            apply(OSVR_DirectionReport const &r) {
                return r.direction;
            }
            static OSVR_DirectionState apply(OSVR_DirectionReport &r) {
                return r.direction;
            }
        };

    } // namespace traits

    /// @brief Generic const accessor for the "state" member of a report.
    template <typename ReportType>
    typename traits::StateType<ReportType>::type const &
    reportState(ReportType const &r) {
        return traits::ReportStateGetter<ReportType>::apply(r);
    }

    /// @brief Generic accessor for the "state" member of a report.
    template <typename ReportType>
    typename traits::StateType<ReportType>::type &reportState(ReportType &r) {
        return traits::ReportStateGetter<ReportType>::apply(r);
    }
} // namespace common
} // namespace osvr

#endif // INCLUDED_ReportState_h_GUID_452FA875_E51C_4A88_5A58_E28CB6F40B86
