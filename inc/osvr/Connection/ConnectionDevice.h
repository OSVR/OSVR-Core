/** @file
    @brief Header

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

#ifndef INCLUDED_ConnectionDevice_h_GUID_C04223D7_51D1_49BE_91AB_73FCA9C88515
#define INCLUDED_ConnectionDevice_h_GUID_C04223D7_51D1_49BE_91AB_73FCA9C88515

// Internal Includes
#include <osvr/Connection/Export.h>
#include <osvr/Connection/ConnectionDevicePtr.h>
#include <osvr/Connection/MessageTypePtr.h>
#include <osvr/Connection/DeviceTokenPtr.h>
#include <osvr/Util/TimeValue.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>
#include <vector>

namespace osvr {
namespace connection {
    /// @brief Base class for connection-specific device data, owned by a
    /// DeviceToken.
    class ConnectionDevice : boost::noncopyable {
      public:
        /// @brief destructor
        OSVR_CONNECTION_EXPORT virtual ~ConnectionDevice();

        typedef std::vector<std::string> NameList;

        /// @brief accessor for (primary) device name
        std::string const &getName() const;

        /// @brief accessor for device names
        NameList const &getNames() const;

        /// @brief Process messages. This shouldn't block.
        ///
        /// Someone needs to call this method frequently.
        void process();

        /// @brief Send message (as primary device name)
        void sendData(util::time::TimeValue const &timestamp, MessageType *type,
                      const char *bytestream, size_t len);

        /// @brief For use only by DeviceToken
        void setDeviceToken(DeviceToken &token);

      protected:
        /// @brief Does this connection device have a device token? Should be
        /// true in nearly every case.
        bool m_hasDeviceToken() const;
        /// @brief accessor for device token
        DeviceToken &m_getDeviceToken();
        /// @brief (Subclass implementation) Process messages. This shouldn't
        /// block.
        virtual void m_process() = 0;

        /// @brief (Subclass implementation) Send message.
        virtual void m_sendData(util::time::TimeValue const &timestamp,
                                MessageType *type, const char *bytestream,
                                size_t len) = 0;

        /// @brief Constructor for use by derived classes only.
        OSVR_CONNECTION_EXPORT ConnectionDevice(std::string const &name);

        /// @brief Constructor for use by derived classes only.
        OSVR_CONNECTION_EXPORT ConnectionDevice(NameList const &names);

      private:
        NameList m_names;
        DeviceToken *m_token;
    };
} // namespace connection
} // namespace osvr
#endif // INCLUDED_ConnectionDevice_h_GUID_C04223D7_51D1_49BE_91AB_73FCA9C88515
