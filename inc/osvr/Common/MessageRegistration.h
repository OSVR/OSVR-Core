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

#ifndef INCLUDED_MessageRegistration_h_GUID_F431F1DB_4193_42AB_3376_C67740E2C6FE
#define INCLUDED_MessageRegistration_h_GUID_F431F1DB_4193_42AB_3376_C67740E2C6FE

// Internal Includes
#include <osvr/Common/RawMessageType.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {
    /// @brief CRTP class template wrapping message-specific data and/or logic.
    ///
    /// @tparam Derived Derived class, your message-specific type: must have a
    /// `static const char * identifier()` method returning the string ID of the
    /// message.
    template <typename Derived> class MessageRegistration {
      public:
        static const char *identifier() { return Derived::identifier(); }

        RawMessageType getMessageType() const { return m_type; }
        void setMessageType(RawMessageType msgType) { m_type = msgType; }

      private:
        Derived &derived() { return *static_cast<Derived *>(this); }
        Derived const &derived() const {
            return *static_cast<Derived const *>(this);
        }

        RawMessageType m_type;
    };
} // namespace common
} // namespace osvr
#endif // INCLUDED_MessageRegistration_h_GUID_F431F1DB_4193_42AB_3376_C67740E2C6FE
