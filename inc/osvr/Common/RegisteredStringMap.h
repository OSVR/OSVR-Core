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

#ifndef INCLUDED_RegisteredStringMap_h_GUID_066235BE_3687_44AD_C2A4_593D6E6780F3
#define INCLUDED_RegisteredStringMap_h_GUID_066235BE_3687_44AD_C2A4_593D6E6780F3

// Internal Includes
#include <osvr/Common/Export.h>
#include <osvr/Util/StringIds.h>

// Library/third-party includes
#include <json/value.h>

// Standard includes
#include <vector>

namespace osvr {
namespace common {

    /// Centralize a string registry. Basically, the server side, and part
    /// of the client side internals.
    class RegisteredStringMap {
      public:
        /// retrieve the ID for the current name or register new ID and return
        /// that
        OSVR_COMMON_EXPORT util::StringID getStringID(std::string const &str);

        /// retrieve the name of the string given the ID
        /// returns empty string if nothing found
        OSVR_COMMON_EXPORT std::string getStringFromId(util::StringID id) const;

        /// Has a new entry been added since the flag was last cleared?
        OSVR_COMMON_EXPORT bool isModified() const;
        /// Clear the modified flag
        OSVR_COMMON_EXPORT void clearModifiedFlag();

        OSVR_COMMON_EXPORT void printCurrentMap();

        OSVR_COMMON_EXPORT std::vector<std::string> getEntries() const;

      protected:
        std::vector<std::string> m_regEntries;

        /// special flag that gets switched whenever new element is inserted;
        bool m_modified = false;
    };

    /// This is like a RegisteredStringMap, except it also knows that some peer
    /// also has a string map, likely with some of the same strings, but with
    /// different ids. Used in reconciliation between server and client since
    /// they are separate entities
    class CorrelatedStringMap {
      public:
        /// retrieve the ID for the current name or register new ID and return
        /// that
        OSVR_COMMON_EXPORT util::StringID getStringID(std::string const &str);

        /// retrieve the name of the string given the ID
        /// returns empty string if nothing found
        OSVR_COMMON_EXPORT std::string getStringFromId(util::StringID id) const;

        /// This is the extra method used by clients, to convert from server's
        /// ids. Will return NULL if peerID to Local ID mapping doesn't exist
        OSVR_COMMON_EXPORT util::StringID
        convertPeerToLocalID(util::PeerStringID peerID) const;

        /// This populates the data structure used by the above method.
        OSVR_COMMON_EXPORT void
        setupPeerMappings(std::vector<std::string> const &peerEntries);

      private:
        RegisteredStringMap m_local;
        /// keeps the peer to local string ID mappings
        std::vector<uint32_t> m_remoteToLocal;
    };
} // namespace common
} // namespace osvr
#endif // INCLUDED_RegisteredStringMap_h_GUID_066235BE_3687_44AD_C2A4_593D6E6780F3
