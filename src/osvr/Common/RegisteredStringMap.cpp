/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/Common/RegisteredStringMap.h>

// Library/third-party includes
#include <boost/algorithm/string.hpp>

// Standard includes
#include <iostream>

namespace osvr {
namespace common {

    /// @brief helper function to print size and contents of the map
    void RegisteredStringMap::printCurrentMap() {
        auto n = m_regEntries.size();
        std::cout << "Current map contains " << m_regEntries.size()
                  << " entries: " << std::endl;
        for (decltype(n) i = 0; i < n; ++i) {
            std::cout << "ID: " << i << "; "
                      << "Name: " << m_regEntries[i] << std::endl;
        }
    }

    util::StringID RegisteredStringMap::getStringID(std::string const &str) {
        auto entry = std::find(begin(m_regEntries), end(m_regEntries), str);
        if (end(m_regEntries) != entry) {
            // we found it.
            return util::StringID(std::distance(begin(m_regEntries), entry));
        }

        // we didn't find an entry in the registry so we'll add a new one
        auto ret = util::StringID(
            m_regEntries.size()); // will be the location of the next insert.
        m_regEntries.push_back(str);
        m_modified = true;
        return ret;
    }

    std::string RegisteredStringMap::getStringFromId(util::StringID id) const {

        // requested non-existent ID (include sanity check)
        if (id.value() >= m_regEntries.size()) {
            // returning empty string
            /// @todo should we throw here?
            return std::string();
        }

        return m_regEntries[id.value()];
    };

    bool RegisteredStringMap::isModified() const { return m_modified; }
    void RegisteredStringMap::clearModifiedFlag() { m_modified = false; }
    std::vector<std::string> RegisteredStringMap::getEntries() const {
        return m_regEntries;
    }

    util::StringID CorrelatedStringMap::getStringID(std::string const &str) {
        return m_local.getStringID(str);
    }

    std::string CorrelatedStringMap::getStringFromId(util::StringID id) const {
        return m_local.getStringFromId(id);
    }

    util::StringID
    CorrelatedStringMap::convertPeerToLocalID(util::PeerStringID peerID) const {
        if (peerID.empty()) {
            return util::StringID();
        }
        if (peerID.value() >= m_remoteToLocal.size()) {
            throw std::out_of_range("Peer ID out of range!");
        }
        return util::StringID(m_remoteToLocal[peerID.value()]);
    }

    void CorrelatedStringMap::setupPeerMappings(
        std::vector<std::string> const &peerEntries) {
        m_remoteToLocal.clear();
        auto n = peerEntries.size();
        for (uint32_t i = 0; i < n; ++i) {
            m_remoteToLocal.push_back(
                m_local.getStringID(peerEntries[i]).value());
        }
    }
}
}
