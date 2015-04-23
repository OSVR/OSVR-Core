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

#ifndef INCLUDED_NetworkingSupport_h_GUID_A5FE2D05_48DC_469E_1013_F2B7736331F3
#define INCLUDED_NetworkingSupport_h_GUID_A5FE2D05_48DC_469E_1013_F2B7736331F3

// Internal Includes
#include <osvr/Common/Export.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>

namespace osvr {
namespace common {
    /// @brief RAII class wrapping networking system startup
    ///
    /// Basically, a clean way of calling WSAStartup() and WSACleanup() at the
    /// right time.
    class NetworkingSupport : boost::noncopyable {
      public:
        /// @brief Constructor
        OSVR_COMMON_EXPORT NetworkingSupport();

        /// @brief Destructor
        OSVR_COMMON_EXPORT ~NetworkingSupport();

        /// @brief Get whether the networking system is successfully "up"
        bool isUp() const { return m_up; }

        /// @brief Get whether the last operation (automatic startup or manual,
        /// early shutdown) was successful.
        bool wasSuccessful() const { return m_success; }

        /// @brief Get error message, if any.
        std::string const &getError() const { return m_err; }

        /// @brief Shutdown before destruction
        void shutdown();

      private:
        /// @brief Platform-specific implementation of starting behavior.
        /// Returns success. Must set m_err on failure.
        bool m_start();
        /// @brief Platform-specific implementation of stopping behavior.
        /// Returns success. Must set m_err on failure.
        bool m_stop();
        bool m_up;
        bool m_success;
        std::string m_err;
    };
} // namespace common
} // namespace osvr

#endif // INCLUDED_NetworkingSupport_h_GUID_A5FE2D05_48DC_469E_1013_F2B7736331F3
