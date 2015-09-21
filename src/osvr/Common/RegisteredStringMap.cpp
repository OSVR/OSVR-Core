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

    /// @brief set the initial number of entries to zero, and flag to update the
    /// Map (since it's new)
    RegisteredStringMap::RegisteredStringMap()
        : m_numEntries(0), m_updateMap(true) {}

    /// @todo add proper destructor
    RegisteredStringMap::~RegisteredStringMap() {}

    /// @brief helper function to print size and contents of the map
    void RegisteredStringMap::printCurrentMap() {

        std::cout << "Current map contains " << m_regEntries.size()
                  << " entries: " << std::endl;
        for (auto &entry : m_regEntries) {
            std::cout << "ID: " << entry.second.value() << "; "
                      << "Name: " << entry.first << std::endl;
        }
    }

    SerializedStringMap RegisteredStringMap::getMap() const {

        Json::Value serializedMap;

        for (auto &entry : m_regEntries) {

            serializedMap[entry.first] = entry.second.value();
        }
        return serializedMap;
    }

    util::StringID
    RegisteredStringMap::registerStringID(std::string const &str) {

        // we've checked the entries before and haven't found so we'll just add
        // new one
        std::string name = str;
        util::StringID newID(m_regEntries.size());

        m_regEntries.insert(std::make_pair(name, newID));
        m_numEntries++;

        m_updateMap = true;
        return newID;
    }

    util::StringID RegisteredStringMap::getStringID(std::string const &str) {

        // check the existing registry first
        for (auto &entry : m_regEntries) {
            // found a matching name, NOTE: CaSe Sensitive
            if (boost::equals(str, entry.first)) {
                m_updateMap = false;
                return entry.second;
            }
        }

        // we didn't find an entry in the registry so we'll add a new one
        return registerStringID(str);
    }

    std::string RegisteredStringMap::getNameFromID(util::StringID &id) const {

        // requested non-existent ID (include sanity check)
        if (id.value() >= m_regEntries.size()) {
            // returning empty string
            return std::string();
        }

        // entries should be ordered 0-.. with new ones
        // appending to the end, so we should be safe at pulling by vector index

        for (auto &entry : m_regEntries) {
            if (entry.second == id) {
                // found name for this ID
                return entry.first;
            }
        }
        // returning empty string
        return std::string();
    };

    bool RegisteredStringMap::isUpdateAvailable() { return m_updateMap; }

    CorrelatedStringMap::CorrelatedStringMap() {}

    /// @todo add proper destructor
    CorrelatedStringMap::~CorrelatedStringMap() {}

    util::StringID
    CorrelatedStringMap::convertPeerToLocalID(util::PeerStringID peerID) const {

        // go thru the mappings and return an empty StringID if nothing's found
        for (auto &mapping : mappings) {

            if (mapping.first == peerID) {
                return mapping.second;
            }
        }

        return util::StringID();
    }

    void CorrelatedStringMap::addPeerToLocalMapping(util::PeerStringID peerID,
                                                    util::StringID localID) {

        // there should be 1 to 1 mapping between peer and local IDs,
        // so if we find one, it should be correct
        for (auto &mapping : mappings) {
            // got a match
            if ((mapping.first == peerID) && (mapping.second == localID)) {
                return;
            }
        }
        // add new mapping
        mappings.push_back(std::make_pair(peerID, localID));
    }

    void CorrelatedStringMap::setupPeerMappings(SerializedStringMap peerData) {

        // go thru the peerData, you get name and id
        // this name may already be stored in correlatedMap
        // so we get the localID first (or register it)
        // then store it in the mappings

        for (auto &name : peerData.getMemberNames()) {

            util::PeerStringID peerID(peerData[name].asUInt());
            util::StringID localID = getStringID(name);
            addPeerToLocalMapping(peerID, localID);
        }
    }
}
}
