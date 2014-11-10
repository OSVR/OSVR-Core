/** @file
    @brief Header declaring the AnyMap structure.

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_AnyMap_h_GUID_794993EB_B778_4E88_16A7_3A047A615587
#define INCLUDED_AnyMap_h_GUID_794993EB_B778_4E88_16A7_3A047A615587

// Internal Includes
#include <ogvr/Util/Export.h>

// Library/third-party includes
#include <boost/any.hpp>

// Standard includes
#include <string>
#include <map>

namespace ogvr {
namespace util {
    /// @brief A data structure storing "any" by name, to reduce coupling.
    class AnyMap {
      public:
        /// @brief Do we have data under this key?
        OGVR_UTIL_EXPORT bool contains(std::string const &key) const;
        /// @overload
        OGVR_UTIL_EXPORT bool contains(const char *key) const;

        /// @brief Set data for the given key
        ///
        /// Silently overwrites existing data at that key.
        /// @{
        OGVR_UTIL_EXPORT void set(std::string const &key,
                                  boost::any const &value);
        OGVR_UTIL_EXPORT void set(const char *key, boost::any const &value);

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
        OGVR_UTIL_EXPORT boost::any get(std::string const &key) const;
        /// @overload
        OGVR_UTIL_EXPORT boost::any get(const char *key) const;

        /// @brief Clears the data for this key.
        ///
        /// If the key doesn't exist, this method does nothing
        OGVR_UTIL_EXPORT void erase(std::string const &key);
        /// @overload
        OGVR_UTIL_EXPORT void erase(const char *key);

      private:
        typedef std::map<std::string, boost::any> Contents;
        Contents m_contents;
    };
} // namespace util
} // namespace ogvr

#endif // INCLUDED_AnyMap_h_GUID_794993EB_B778_4E88_16A7_3A047A615587
