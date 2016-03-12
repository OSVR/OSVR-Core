/** @file
    @brief Header

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
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

#ifndef INCLUDED_ConnectionWarning_h_GUID_6A6C43FA_A765_4C85_ADB0_950995431B8B
#define INCLUDED_ConnectionWarning_h_GUID_6A6C43FA_A765_4C85_ADB0_950995431B8B

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

// Internal Includes
// - none

// Library/third-party includes
#include <boost/noncopyable.hpp>

#include <asio.hpp>
#include <asio/io_service.hpp>

// Standard includes
#include <array>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <set>

namespace osvr {
namespace server {

    struct ConnectionAttempt {
        enum class Protocol { TCP, UDP };
        Protocol protocol;
        unsigned short port;
        /// An empty string means a loopback - localhost - address.
        std::string address;
    };

    inline bool operator<(ConnectionAttempt const &lhs,
                          ConnectionAttempt const &rhs) {
        /// first compare protocol
        if (lhs.protocol == ConnectionAttempt::Protocol::TCP &&
            rhs.protocol == ConnectionAttempt::Protocol::UDP) {
            return true;
        }
        if (lhs.protocol != rhs.protocol) {
            // order is reversed, then.
            return false;
        }
        /// then compare port
        if (lhs.port < rhs.port) {
            return true;
        }
        if (lhs.port != rhs.port) {
            // order is reversed, then.
            return false;
        }
        /// Finally, compare the address: save the expensive for last.
        return (lhs.address < rhs.address);
    }

    inline bool operator==(ConnectionAttempt const &lhs,
                           ConnectionAttempt const &rhs) {
        return (lhs.protocol == rhs.protocol) && (lhs.port == rhs.port) &&
               (lhs.address == rhs.address);
    }

    /// Opens a port for TCP and UDP, and on attempts to connect or receipt of a
    /// datagram, lets the owning object know.
    class ConnectionWarning : boost::noncopyable {
      public:
        ConnectionWarning(unsigned short port,
                          std::string const &iface = std::string("127.0.0.1"));

        bool openedUdp() const { return m_openedUdp; }
        bool openedTcp() const { return m_openedTcp; }

        /// returns true if one or more apparently new attempts to connect were
        /// detected.
        bool process();
        /// Gets a list of all connection attempts processed during the last
        /// cycle.
        std::vector<ConnectionAttempt> const &getAllAttempts() const {
            return m_attempts;
        }

        /// Gets a list of connection attempts processed during the last cycle
        /// that came from a unique host, port, and protocol
        std::vector<ConnectionAttempt> const &getNewAttempts() const {
            return m_newAttempts;
        }

        void stop() { m_context.stop(); }

      private:
        static void displayError(const char *action,
                                 asio::error_code const &ec) {
            if (ec) {
                std::cerr << "***Got an error " << action << ": "
                          << ec.message() << std::endl;
            }
        }
        void initUdp(unsigned short port, asio::ip::address const &addr);

        void initTcp(unsigned short port, asio::ip::address const &addr);

        void startUdpReceive();

        void startTcpAccept();

        void logAttempt(ConnectionAttempt const &attempt);

        template <typename Endpoint>
        void addEndpointInfoToStream(std::ostream &os,
                                     Endpoint const &endpoint) {
            if (endpoint.address().is_loopback()) {
                os << " from an app on this computer";
            } else {
                os << " from an app on " << endpoint.address().to_string();
            }
        }

        void receiveUdp(const asio::error_code &error);

        asio::io_context m_context;

        bool m_openedUdp = false;
        asio::ip::udp::socket m_udpSocket;
        asio::ip::udp::endpoint m_udpRemoteEndpoint;
        std::array<char, 1700> m_udpBuf;

        bool m_openedTcp = false;
        asio::ip::tcp::acceptor m_tcpAcceptor;

        std::vector<ConnectionAttempt> m_attempts;
        std::vector<ConnectionAttempt> m_newAttempts;

        /// @todo may need to clear this periodically to avoid continually
        /// growing!
        std::set<ConnectionAttempt> m_knownAttempts;
    };

} // namespace server
} // namespace osvr
#endif // INCLUDED_ConnectionWarning_h_GUID_6A6C43FA_A765_4C85_ADB0_950995431B8B
