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

#ifndef INCLUDED_RawMessageType_h_GUID_839F0B4F_18AC_468C_31DB_33601F95DA3C
#define INCLUDED_RawMessageType_h_GUID_839F0B4F_18AC_468C_31DB_33601F95DA3C

// Internal Includes
#include <osvr/Common/Export.h>
#include <osvr/Util/StdInt.h>

// Library/third-party includes
#include <boost/optional.hpp>

// Standard includes
// - none

namespace osvr {
namespace common {
    /// @brief Type-safe wrapper with built-in default for a VRPN "message type"
    /// integer.
    ///
    /// @todo add test code to ensure that the default value matches
    /// vrpn_ANY_TYPE as found in vrpn_Connection.h
    class RawMessageType {
      public:
        typedef int32_t UnderlyingMessageType;
        /// @brief Default constructor - "any message type"
        RawMessageType();

        /// @brief Constructor from a registered message type
        explicit RawMessageType(UnderlyingMessageType msg);

        /// @brief Gets the registered message type value or default
        UnderlyingMessageType get() const;

        /// @brief Gets the registered message type value, if specified,
        /// otherwise returns the provided value.
        UnderlyingMessageType getOr(UnderlyingMessageType valueIfNotSet) const;

      private:
        boost::optional<UnderlyingMessageType> m_message;
    };

} // namespace common
} // namespace osvr

#endif // INCLUDED_RawMessageType_h_GUID_839F0B4F_18AC_468C_31DB_33601F95DA3C
