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
#include <iostream>
#include <sstream>

namespace osvr {
namespace server {
    /// Opens a port for TCP and UDP, and on attempts to connect or receipt of a
    /// datagram, lets the owning object know.
    class ConnectionWarning : boost::noncopyable {
      public:
        ConnectionWarning(unsigned short port)
            : m_udpSocket(m_context), m_tcpAcceptor(m_context),
              m_tcpSocket(m_context) {
            asio::error_code ec;

            m_udpSocket.open(asio::ip::udp::v4());
            m_udpSocket.bind(asio::ip::udp::endpoint(
                                 asio::ip::make_address("localhost"), port),
                             ec);
            if (!ec) {
                // successfully bound the UDP socket
                m_openedUdp = true;
                startUdpReceive();
            }

            m_tcpAcceptor.open(asio::ip::tcp::v4());
            m_tcpAcceptor.set_option(
                asio::ip::tcp::acceptor::reuse_address(true));
            m_tcpAcceptor.bind(asio::ip::tcp::endpoint(
                                   asio::ip::make_address("localhost"), port),
                               ec);
            if (!ec) {
                // successfully bound the TCP acceptor
                m_openedTcp = true;
                startTcpAccept();
            }
        }

        bool openedUdp() const { return m_openedUdp; }
        bool openedTcp() const { return m_openedTcp; }

        std::vector<std::string> const &process() {
            m_attempts.clear();
            /// Like .run(), but doesn't block
            asio::error_code ec;
            m_context.poll(ec);
            if (ec) {
                std::cout << "Got error trying to poll the ASIO context: "
                          << ec.message() << std::endl;
            }
            return m_attempts;
        }

      private:
        void startUdpReceive() {
            m_udpSocket.async_receive_from(
                asio::buffer(m_udpBuf), m_udpRemoteEndpoint,
                [&](const asio::error_code &error, std::size_t) {
                    receiveUdp(error);
                });
        }
        void startTcpAccept() {
            m_tcpAcceptor.async_accept(
                m_tcpSocket, m_tcpRemoteEndpoint,
                [&](const asio::error_code &error) { acceptTcp(error); });
        }
        template <typename Endpoint>
        void addEndpointInfoToStream(std::ostream &os, Endpoint &endpoint) {
            if (endpoint.address().is_loopback()) {
                os << " from an app on this computer";
            } else {
                os << " from an app on " << endpoint.address().to_string();
            }
        }
        void receiveUdp(const asio::error_code &error) {
            if (!error || error == asio::error::message_size) {
                std::ostringstream os;
                os << "Got UDP connection attempt";
                addEndpointInfoToStream(os, m_udpRemoteEndpoint);
                std::cout << "***" << os.str() << std::endl;
                m_attempts.emplace_back(os.str());
            }
            startUdpReceive();
        }
        void acceptTcp(const asio::error_code &error) {
            if (!error) {

                std::ostringstream os;
                os << "Got TCP connection attempt";
                addEndpointInfoToStream(os, m_tcpRemoteEndpoint);
                std::cout << "***" << os.str() << std::endl;
                m_attempts.emplace_back(os.str());

                // Unceremoniously dump the client. Sorry.
                m_tcpSocket.close();
            }
            startTcpAccept();
        }

        asio::io_context m_context;

        bool m_openedUdp = false;
        asio::ip::udp::socket m_udpSocket;
        asio::ip::udp::endpoint m_udpRemoteEndpoint;
        // std::array<char, 1> m_udpBuf;
        char m_udpBuf[1];

        bool m_openedTcp = false;
        asio::ip::tcp::acceptor m_tcpAcceptor;
        asio::ip::tcp::socket m_tcpSocket;
        asio::ip::tcp::endpoint m_tcpRemoteEndpoint;

        std::vector<std::string> m_attempts;
        bool m_gotConnectAttempt = false;
    };

} // namespace server
} // namespace osvr
#endif // INCLUDED_ConnectionWarning_h_GUID_6A6C43FA_A765_4C85_ADB0_950995431B8B
