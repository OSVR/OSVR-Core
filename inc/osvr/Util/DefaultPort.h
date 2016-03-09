/** @file
    @brief Header with default port numbers for OSVR and VRPN. These constants
   are specified as enums, but named concisely and in "PascalStyle" to avoid
   collision with ALL_CAPS potential defines, which is safe since they are
   scoped.

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

#ifndef INCLUDED_DefaultPort_h_GUID_6DE29AB7_FE01_47BE_93BA_A1BA7589DD0C
#define INCLUDED_DefaultPort_h_GUID_6DE29AB7_FE01_47BE_93BA_A1BA7589DD0C

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace util {
    enum {
        /// @todo Update this for 1.0!
        DefaultOSVRPort = 3883,
        DefaultVRPNPort = 3883
    };
} // namespace util
} // namespace osvr

#endif // INCLUDED_DefaultPort_h_GUID_6DE29AB7_FE01_47BE_93BA_A1BA7589DD0C
