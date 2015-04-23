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

#ifndef INCLUDED_ReportStateTraits_h_GUID_44BA0D27_B1B4_4A8C_5D47_2AFECC2C2BEE
#define INCLUDED_ReportStateTraits_h_GUID_44BA0D27_B1B4_4A8C_5D47_2AFECC2C2BEE

// Internal Includes
#include <osvr/Util/ClientReportTypesC.h>

// Library/third-party includes
// - none

// Standard includes
#include <type_traits>

namespace osvr {
namespace common {
    namespace traits {
        /// @brief Type predicate: Whether callbacks of a report type should
        /// store state for that type.
        template <typename T> struct KeepStateForReport : std::true_type {};

        /// @brief Don't store the imaging reports as state, since doing so
        /// would involve extra lifetime management.
        template <>
        struct KeepStateForReport<OSVR_ImagingReport> : std::false_type {};

    } // namespace traits

} // namespace common
} // namespace osvr
#endif // INCLUDED_ReportStateTraits_h_GUID_44BA0D27_B1B4_4A8C_5D47_2AFECC2C2BEE
