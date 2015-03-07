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
#include "VRPNMultiserver.h"

// Library/third-party includes
// - none

// Standard includes
#include <iostream>
#include <sstream>

std::string VRPNMultiserverData::getName(std::string const &nameStem) {
    size_t num = assignNumber(nameStem);
    std::ostringstream os;
    os << nameStem << num;
    return os.str();
}

size_t VRPNMultiserverData::assignNumber(std::string const &nameStem) {
    auto it = m_nameCount.find(nameStem);
    if (it != end(m_nameCount)) {
        it->second++;
        return it->second;
    }
    m_nameCount[nameStem] = 0;
    return 0;
}
