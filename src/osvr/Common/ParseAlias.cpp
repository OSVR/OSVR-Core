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
#include <osvr/Common/ParseAlias.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <json/reader.h>
#include <json/writer.h>

// Standard includes
#include <sstream>

namespace osvr {
namespace common {

    /// @brief Helper, converts old-style tracker source into normal. For a
    /// little backward-compatibility.
    ///
    /// @todo remove this method in the future.
    inline std::string getPathFromOldRouteSource(Json::Value obj) {
        std::ostringstream ret;
        if (obj.isObject() && obj.isMember("tracker")) {
            auto tracker = obj["tracker"].asString();
            if (tracker.front() != '/') {
                ret << "/";
            }
            ret << tracker;
            ret << "/tracker";
            if (obj.isMember("sensor")) {
                ret << "/";
                ret << obj["sensor"].asInt();
            }
        }
        return ret.str();
    }

    ParsedAlias::ParsedAlias(std::string const &src) : m_simple(true) {
        m_parse(src);
    }

    ParsedAlias::ParsedAlias(Json::Value src) : m_simple(true) { m_parse(src); }

    bool ParsedAlias::isValid() const { return !m_value.isNull(); }

    bool ParsedAlias::isSimple() const { return m_simple; }
    std::string ParsedAlias::getLeaf() const { return m_leaf().asString(); }
    void ParsedAlias::setLeaf(std::string const &leaf) { m_leaf() = leaf; }

    std::string ParsedAlias::getAlias() const {
        if (m_value.isString()) {
            return m_value.asString();
        }
        Json::FastWriter writer;
        std::string ret = writer.write(m_value);
        // Remove trailing line feed, if any.
        while (ret.back() == '\n' || ret.back() == '\r') {
            ret.pop_back();
        }
        return ret;
    }

    Json::Value ParsedAlias::getAliasValue() const { return m_value; }

    void ParsedAlias::m_parse(std::string const &src) {
        Json::Value val;
        Json::Reader reader;
        if (!reader.parse(src, val)) {
            // If it didn't parse as JSON, just assume it's a string.
            m_value = src;
            return;
        }
        m_parse(val);
    }

    static const char CHILD_KEY[] = "child";
    static const char SOURCE_KEY[] = "source";
    void ParsedAlias::m_parse(Json::Value &val) {
        if (val.isString()) {
            // Assume a string is just a string.
            m_value = val;
            return;
        }
        if (val.isObject()) {
            if (val.isMember(SOURCE_KEY)) {
                // Strip any initial "source" level
                m_parse(val[SOURCE_KEY]);
                return;
            }

            // Assume an object means a transform.
            m_simple = false;
            m_value = val;

            auto &leaf = m_leaf();
            if (leaf.isString()) {
                return;
            }

            auto trackerEquiv = getPathFromOldRouteSource(leaf);
            if (!trackerEquiv.empty()) {
                leaf = trackerEquiv;
                return;
            }

            OSVR_DEV_VERBOSE(
                "Couldn't handle transform leaf: " << leaf.toStyledString());
        }
        m_value = Json::nullValue;
        /// @todo finish by throwing?
    }
    Json::Value &ParsedAlias::m_leaf() {
        Json::Value *current = &m_value;
        while (current->isObject() && current->isMember(CHILD_KEY)) {
            current = &(*current)[CHILD_KEY];
        }
        return *current;
    }
    Json::Value const &ParsedAlias::m_leaf() const {
        Json::Value const *current = &m_value;
        while (current->isObject() && current->isMember(CHILD_KEY)) {
            current = &(*current)[CHILD_KEY];
        }
        return *current;
    }

} // namespace common
} // namespace osvr