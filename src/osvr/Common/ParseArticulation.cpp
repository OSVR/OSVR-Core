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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/Common/ParseArticulation.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <json/reader.h>

// Standard includes

namespace osvr {
namespace common {

    ParsedArticulation::ParsedArticulation(std::string const &src) {
        m_parse(src);
    }

    ParsedArticulation::ParsedArticulation(Json::Value src) { m_parse(src); }

    bool ParsedArticulation::isValid() const {
        /// @todo should I check for more conditions here
        return !m_trackerPath.empty();
    }

    std::string ParsedArticulation::getBoneName() const {
        // return a simple string
        return m_boneName;
    }

    std::string ParsedArticulation::getArticulationType() const {
        // return a simple string
        return m_articulationType;
    }

    std::string ParsedArticulation::getTrackerPath() const {
        return m_trackerPath;
    }

    void
    ParsedArticulation::setTrackerPath(std::string const &fullTrackerPath) {
        m_trackerPath = fullTrackerPath;
    }

    void ParsedArticulation::m_parse(std::string const &src) {
        Json::Value val;
        Json::Reader reader;
        if (!reader.parse(src, val)) {
            // If it didn't parse as JSON, just assume it's a string.
            m_trackerPath = src;
            return;
        }
        m_parse(val);
    }

    static const char DATA_KEY[] = "data";
    static const char BONE_NAME_KEY[] = "boneName";
    static const char ARTIC_TYPE_KEY[] = "type";
    void ParsedArticulation::m_parse(Json::Value &val) {
        if (val.isString()) {
            // Assume a string is just a string.
            m_trackerPath = val.asCString();
            return;
        }
        if (val.isObject()) {

            m_trackerPath = (val[DATA_KEY].isNull() && val[DATA_KEY].isString())
                                ? std::string()
                                : val[DATA_KEY].asString();
            m_boneName =
                (val[BONE_NAME_KEY].isNull() && val[BONE_NAME_KEY].isString())
                    ? std::string()
                    : val[BONE_NAME_KEY].asString();
            m_trackerPath = (val[DATA_KEY].isNull() && val[DATA_KEY].isString())
                                ? std::string()
                                : val[DATA_KEY].asString();
            return;
        }
    }

} // namespace common
} // namespace osvr