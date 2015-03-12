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

#ifndef INCLUDED_GetEnvironmentVariable_h_GUID_500031B7_31FB_45A1_B1E9_8E61905A4E0F
#define INCLUDED_GetEnvironmentVariable_h_GUID_500031B7_31FB_45A1_B1E9_8E61905A4E0F

// Internal Includes
#include <osvr/Common/Export.h>

// Library/third-party includes
#include <boost/optional.hpp>

// Standard includes
#include <string>

namespace osvr {
namespace common {
    /// @brief Gets an environment variable's value. On systems that don't
    /// distinguish between having a variable defined and having it non-empty
    /// (like Windows), empty will imply not defined and thus an empty
    /// boost::optional return value.
    OSVR_COMMON_EXPORT boost::optional<std::string>
    getEnvironmentVariable(std::string const &var);
} // namespace common
} // namespace osvr
#endif // INCLUDED_GetEnvironmentVariable_h_GUID_500031B7_31FB_45A1_B1E9_8E61905A4E0F
