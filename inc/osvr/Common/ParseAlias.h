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

#ifndef INCLUDED_ParseAlias_h_GUID_F34B71BE_C378_4999_CE65_16AC5D1A80F1
#define INCLUDED_ParseAlias_h_GUID_F34B71BE_C378_4999_CE65_16AC5D1A80F1

// Internal Includes
#include <osvr/Common/Export.h>

// Library/third-party includes
#include <json/value.h>

// Standard includes
#include <string>

namespace osvr {
namespace common {
    class ParsedAlias {
      public:
        /// @brief Constructor - performs parse and normalization of format.
        OSVR_COMMON_EXPORT ParsedAlias(std::string const &src);

        /// @brief Constructor - performs normalization of format.
        ParsedAlias(Json::Value src);

        /// @brief Did the alias parse in a valid way?
        OSVR_COMMON_EXPORT bool isValid() const;

        /// @brief Is this a simple (string-only, no transform) alias?
        OSVR_COMMON_EXPORT bool isSimple() const;

        /// @brief Get the ultimate source/leaf of the alias
        OSVR_COMMON_EXPORT std::string getLeaf() const;

        /// @brief Set the leaf of the alias: should be an absolute path.
        void setLeaf(std::string const &leaf);

        /// @brief Get the normalized, cleaned, compacted version of the alias.
        OSVR_COMMON_EXPORT std::string getAlias() const;

        /// @brief Gets a copy of the normalized version of the alias as a
        /// Json::Value
        OSVR_COMMON_EXPORT Json::Value getAliasValue() const;

      private:
        void m_parse(std::string const &src);
        void m_parse(Json::Value &val);
        Json::Value &m_leaf();
        Json::Value const &m_leaf() const;
        bool m_simple;
        Json::Value m_value;
    };
} // namespace common
} // namespace osvr
#endif // INCLUDED_ParseAlias_h_GUID_F34B71BE_C378_4999_CE65_16AC5D1A80F1
