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

#ifndef INCLUDED_ReportTypeWrapper_h_GUID_D9BF9925_4EA6_4CB5_4035_332F8D5307F2
#define INCLUDED_ReportTypeWrapper_h_GUID_D9BF9925_4EA6_4CB5_4035_332F8D5307F2

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace client {
    namespace traits {
        /// @brief A template type used as a function argument for type
        /// deduction or overloading.
        template <typename ReportType> struct ReportTypeWrapper;

        /// @brief Creates and returns a null pointer to a given report type
        /// wrapper, for use as a function argument for type deduction or
        /// overloading.
        template <typename ReportType>
        inline ReportTypeWrapper<ReportType> *report() {
            return static_cast<ReportTypeWrapper<ReportType> *>(nullptr);
        }
    } // namespace traits

} // namespace client
} // namespace osvr

#endif // INCLUDED_ReportTypeWrapper_h_GUID_D9BF9925_4EA6_4CB5_4035_332F8D5307F2
