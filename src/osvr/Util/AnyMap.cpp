/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/Util/AnyMap.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace util {
    /// All the const char * overloads are in case we change the internal
    /// representation to something that works faster with string literals.
    bool AnyMap::contains(std::string const &key) const {
        return (m_contents.count(key) == 1);
    }

    bool AnyMap::contains(const char *key) const {
        return contains(std::string(key));
    }

    void AnyMap::set(std::string const &key, boost::any const &value) {
        m_contents.insert(std::make_pair(key, value));
    }

    void AnyMap::set(const char *key, boost::any const &value) {
        set(std::string(key), value);
    }

    boost::any AnyMap::get(std::string const &key) const {
        Contents::const_iterator location = m_contents.find(key);
        if (location == m_contents.end()) {
            return boost::any();
        }
        return location->second;
    }

    boost::any AnyMap::get(const char *key) const {
        return get(std::string(key));
    }

    void AnyMap::erase(std::string const &key) {
        Contents::iterator location = m_contents.find(key);
        if (location != m_contents.end()) {
            m_contents.erase(location);
        }
    }

    void AnyMap::erase(const char *key) { erase(std::string(key)); }
} // namespace util
} // namespace osvr
