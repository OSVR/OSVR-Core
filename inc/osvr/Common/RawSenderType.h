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

#ifndef INCLUDED_RawSenderType_h_GUID_DF724F49_FE2F_4EB4_8CFC_8C16424400EC
#define INCLUDED_RawSenderType_h_GUID_DF724F49_FE2F_4EB4_8CFC_8C16424400EC

// Internal Includes
#include <osvr/Common/Export.h>
#include <osvr/Util/StdInt.h>

// Library/third-party includes
#include <boost/optional.hpp>

// Standard includes
// - none

namespace osvr {
namespace common {
    /// @brief Type-safe wrapper with built-in default for a VRPN "sender type"
    /// integer.
    ///
    /// @todo add test code to ensure that the default value matches
    /// vrpn_ANY_SENDER as found in vrpn_Connection.h
    class RawSenderType {
      public:
        typedef int32_t UnderlyingSenderType;

        /// @brief Default constructor - "any sender"
        RawSenderType();

        /// @brief Constructor from a registered sender
        OSVR_COMMON_EXPORT explicit RawSenderType(UnderlyingSenderType sender);

        /// @brief Gets the registered sender value or default
        UnderlyingSenderType get() const;

        /// @brief Gets the registered sender value, if specified, otherwise
        /// returns the provided value.
        UnderlyingSenderType getOr(UnderlyingSenderType valueIfNotSet) const;

      private:
        boost::optional<UnderlyingSenderType> m_sender;
    };

} // namespace common
} // namespace osvr
#endif // INCLUDED_RawSenderType_h_GUID_DF724F49_FE2F_4EB4_8CFC_8C16424400EC
