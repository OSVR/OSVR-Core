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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_ParseArticulation_h_GUID_A3F32F4D_D900_46BB_027D_C45E437E1F8C
#define INCLUDED_ParseArticulation_h_GUID_A3F32F4D_D900_46BB_027D_C45E437E1F8C

// Internal Includes
#include <osvr/Common/Export.h>

// Library/third-party includes
#include <json/value.h>

// Standard includes
#include <string>

namespace osvr {
namespace common {
    class ParsedArticulation {
      public:
        /// @brief Constructor - performs parse and normalization of format.
        OSVR_COMMON_EXPORT ParsedArticulation(std::string const &src);

        /// @brief Constructor - performs normalization of format.
        ParsedArticulation(Json::Value src);

        /// @brief Will return false if trackerPath is not specified. Other
        /// details are optional
        OSVR_COMMON_EXPORT bool isValid() const;

        /// @brief Get the ultimate source/leaf of the articulation. May return
        /// an empty string if it it's not specified
        OSVR_COMMON_EXPORT std::string getBoneName() const;

        /// @brief Get the type of articulation. Returns an empty string if it's
        /// not specified
        OSVR_COMMON_EXPORT std::string getArticulationType() const;

        /// @brief Get the tracker path to sensor that corresponds to current
        /// articulation. Tracker path must be specified during the creation and
        /// will make articulation invalid
        OSVR_COMMON_EXPORT std::string getTrackerPath() const;

        /// @brief Sets the tracker path. After getting the path from
        /// articulation spec, it should be converted to full path (with device
        /// name)
        OSVR_COMMON_EXPORT void
        setTrackerPath(std::string const &fullTrackerPath);

      private:
        void m_parse(std::string const &src);
        void m_parse(Json::Value &val);
        std::string m_boneName;
        std::string m_articulationType;
        std::string m_trackerPath;
        Json::Value m_value;
    };
} // namespace common
} // namespace osvr

#endif // INCLUDED_ParseArticulation_h_GUID_A3F32F4D_D900_46BB_027D_C45E437E1F8C
