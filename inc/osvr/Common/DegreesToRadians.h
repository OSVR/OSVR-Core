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

#ifndef INCLUDED_DegreesToRadians_h_GUID_8354A4E2_30FF_429C_2569_E83EEF10E13A
#define INCLUDED_DegreesToRadians_h_GUID_8354A4E2_30FF_429C_2569_E83EEF10E13A

// Internal Includes
// - none

// Library/third-party includes
#include <boost/math/constants/constants.hpp>

// Standard includes
// - none

namespace osvr {
namespace common {
    inline double degreesToRadians(double degrees) {
        using namespace boost::math::double_constants;
        return degrees * pi / 180.0;
    }
} // namespace common
} // namespace osvr
#endif // INCLUDED_DegreesToRadians_h_GUID_8354A4E2_30FF_429C_2569_E83EEF10E13A
