/** @file
    @brief Header defining some special values that may be passed to some
   functions that request a port number to potentially append to a host
   specification.  These constants are specified as enums, but named
   concisely and in "PascalStyle" to avoid collision with ALL_CAPS potential
   defines, which is safe since they are scoped.

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

#ifndef INCLUDED_PortFlags_h_GUID_F7D61616_C2D5_4795_95D2_DDE2A296CE01
#define INCLUDED_PortFlags_h_GUID_F7D61616_C2D5_4795_95D2_DDE2A296CE01

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
// - none
namespace osvr {
namespace util {

    enum {
        UseDefaultPort = -1,
        /// For use when there may not actually be a port in use: some single
        /// process situations, etc.
        OmitAppendingPort = -2
    };

} // namespace util
} // namespace osvr
#endif // INCLUDED_PortFlags_h_GUID_F7D61616_C2D5_4795_95D2_DDE2A296CE01
