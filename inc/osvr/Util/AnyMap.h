/** @file
    @brief Header declaring the osvr::util::AnyMap structure.

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

#ifndef INCLUDED_AnyMap_h_GUID_794993EB_B778_4E88_16A7_3A047A615587
#define INCLUDED_AnyMap_h_GUID_794993EB_B778_4E88_16A7_3A047A615587

// Internal Includes
#include <osvr/Util/Export.h>
#include <osvr/Util/AnyMap_fwd.h>

// Library/third-party includes
#include <boost/any.hpp>

// Standard includes
#include <string>
#include <map>

namespace osvr {
namespace util {
    /// @brief A data structure storing "any" by name, to reduce coupling.
    class AnyMap {
      public:
        /// @brief Do we have data under this key?
        OSVR_UTIL_EXPORT bool contains(std::string const &key) const;
        /// @overload
        OSVR_UTIL_EXPORT bool contains(const char *key) const;

        /// @brief Set data for the given key
        ///
        /// Silently overwrites existing data at that key.
        /// @{
        OSVR_UTIL_EXPORT void set(std::string const &key,
                                  boost::any const &value);
        OSVR_UTIL_EXPORT void set(const char *key, boost::any const &value);

        template <typename T> void set(std::string const &key, T value) {
            set(key, boost::any(value));
        }

        template <typename T> void set(const char *key, T value) {
            set(key, boost::any(value));
        }
        /// @}

        /// @brief Get the data for this key.
        ///
        /// Returns an empty boost::any if the key doesn't exist.
        OSVR_UTIL_EXPORT boost::any get(std::string const &key) const;
        /// @overload
        OSVR_UTIL_EXPORT boost::any get(const char *key) const;

        /// @brief Clears the data for this key.
        ///
        /// If the key doesn't exist, this method does nothing
        OSVR_UTIL_EXPORT void erase(std::string const &key);
        /// @overload
        OSVR_UTIL_EXPORT void erase(const char *key);

      private:
        typedef std::map<std::string, boost::any> Contents;
        Contents m_contents;
    };
} // namespace util
} // namespace osvr

#endif // INCLUDED_AnyMap_h_GUID_794993EB_B778_4E88_16A7_3A047A615587
