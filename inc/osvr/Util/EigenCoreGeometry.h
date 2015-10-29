/** @file
    @brief Header wrapping include of <Eigen/Core> and <Eigen/Geometry> for
   warning quieting.

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

#ifndef INCLUDED_EigenCoreGeometry_h_GUID_84777061_9532_4BBD_0ADC_3E53E516AE23
#define INCLUDED_EigenCoreGeometry_h_GUID_84777061_9532_4BBD_0ADC_3E53E516AE23

#if 0 // warning suppressions no longer needed with currently-used Eigen version
#ifdef _MSC_VER
#define OSVR_MSVC_WARNINGS_PUSHED
#pragma warning(push)
#pragma warning(disable : 4127)
#endif
#endif

#if defined(__clang__) ||                                                      \
    (defined(__GNUC__) && (((__GNUC__ * 100) + __GNUC_MINOR__) >= 406))
// Suppress std::binder1st/std::binder2nd deprecation notices when building as
// new c++
#define OSVR_GCC_WARNINGS_PUSHED
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <Eigen/Core>
#include <Eigen/Geometry>

#ifdef OSVR_GCC_WARNINGS_PUSHED
#undef OSVR_GCC_WARNINGS_PUSHED
#pragma GCC diagnostic pop
#endif

#ifdef OSVR_MSVC_WARNINGS_PUSHED
#undef OSVR_MSVC_WARNINGS_PUSHED
#pragma warning(pop)
#endif

#endif // INCLUDED_EigenCoreGeometry_h_GUID_84777061_9532_4BBD_0ADC_3E53E516AE23
