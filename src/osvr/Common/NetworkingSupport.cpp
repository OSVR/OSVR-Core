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
#include <osvr/Common/NetworkingSupport.h>
#include <osvr/Util/WideToUTF8.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>
#include <type_traits>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>

#endif

namespace osvr {
namespace common {

#ifdef _WIN32

    static inline std::string wsaErrorCodeToString(int err) {
        TCHAR buf[256] = {0};

        FormatMessage(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
            err, 0, &(buf[0]), sizeof(buf) / sizeof(buf[0]), NULL);
        return util::tcharToUTF8String(buf);
    }

    inline bool NetworkingSupport::m_start() {
        WSADATA wsaData;
        auto status = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (0 == status) {
            m_up = true;
            return true;
        }
        m_err = "WSAStartup failed: " + wsaErrorCodeToString(status);
        return false;
    }

    inline bool NetworkingSupport::m_stop() {
        if (SOCKET_ERROR == WSACleanup()) {
            m_err =
                "WSACleanup failed: " + wsaErrorCodeToString(WSAGetLastError());
            return false;
        }
        return true;
    }
#else
    // Don't think any other platforms need this.
    inline bool NetworkingSupport::m_start() { return true; }
    inline bool NetworkingSupport::m_stop() { return true; }
#endif

    NetworkingSupport::NetworkingSupport() : m_up(false), m_success(true) {
        m_success = m_start();
    }

    NetworkingSupport::~NetworkingSupport() {
        shutdown();
        if (!wasSuccessful()) {
            std::cerr << getError() << std::endl;
        }
    }

    void NetworkingSupport::shutdown() {
        m_success = true;
        m_err.clear();
        if (m_up) {
            m_success = m_stop();
        }
    }
} // namespace common
} // namespace osvr