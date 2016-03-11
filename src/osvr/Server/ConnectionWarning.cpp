/** @file
    @brief Implementation

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

// Internal Includes
#include "ConnectionWarning.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace server {
    ConnectionWarning::ConnectionWarning(unsigned short port,
                                         std::string const &iface)
        : m_udpSocket(m_context, asio::ip::udp::v4()),
          m_tcpAcceptor(m_context, asio::ip::tcp::v4()) {
        auto addr = asio::ip::make_address(iface);
        std::cout << "*** Creating ConnectionWarning on " << addr.to_string()
                  << " port " << port << std::endl;
        initUdp(port, addr);
        initTcp(port, addr);
    }
    std::vector<std::string> const &osvr::server::ConnectionWarning::process() {
        m_attempts.clear();
        /// Like .run(), but doesn't block
        asio::error_code ec;
        m_context.poll(ec);
        if (ec) {
            displayError("trying to poll the ASIO context", ec);
        }
        return m_attempts;
    }

    inline void ConnectionWarning::initUdp(unsigned short port,
                                           asio::ip::address const &addr) {
        using asio::ip::udp;
        asio::error_code ec;

        /// local only
        m_udpSocket.set_option(udp::socket::do_not_route(true));

        /// reuse address
        /// Don't really care about errors in setting this option, which is why
        /// we're just passing the error code and ignoring it.
        m_udpSocket.set_option(udp::socket::reuse_address(true), ec);

        /// Bind to address and port. This we at least want to know if we
        /// succeeded at.
        m_udpSocket.bind(udp::endpoint(addr, port), ec);
        if (ec) {
            displayError("binding UDP socket to port", ec);
            return;
        }
        // successfully bound the UDP socket
        m_openedUdp = true;
        startUdpReceive();
    }

    inline void ConnectionWarning::initTcp(unsigned short port,
                                           asio::ip::address const &addr) {
        using asio::ip::tcp;
        asio::error_code ec;

        /// local only
        m_tcpAcceptor.set_option(tcp::acceptor::do_not_route(true));

        /// reuse address
        /// Don't really care about errors in setting this option, which is why
        /// we're just passing the error code and ignoring it.
        m_tcpAcceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true),
                                 ec);

        /// Bind to address and port. This we at least want to know if we
        /// succeeded at.
        m_tcpAcceptor.bind(tcp::endpoint(addr, port), ec);

        if (ec) {
            displayError("binding the TCP acceptor", ec);
            return;
        }
        m_tcpAcceptor.listen();
        // successfully bound the TCP acceptor
        m_openedTcp = true;
        startTcpAccept();
    }

    inline void ConnectionWarning::startUdpReceive() {
        /// @todo Asio needs std::array support in their asio::buffer
        /// constructor - adjust this when that's available.
        m_udpSocket.async_receive_from(
            asio::buffer(m_udpBuf.data(), m_udpBuf.size()), m_udpRemoteEndpoint,
            [&](const asio::error_code &error, std::size_t) {
                receiveUdp(error);
            });
    }

    inline void ConnectionWarning::startTcpAccept() {
        using asio::ip::tcp;
        m_tcpAcceptor.async_accept([&](std::error_code ec, tcp::socket socket) {
            if (!ec) {
                /// Log the connection.
                std::ostringstream os;
                os << "Got TCP connection attempt";
                addEndpointInfoToStream(os, socket.remote_endpoint());
                std::cout << "***" << os.str() << std::endl;
                m_attempts.emplace_back(os.str());

                /// Shut down the socket, both ends.
                asio::error_code error;
                socket.shutdown(tcp::socket::shutdown_both, error);
                if (error) {
                    displayError("shut down TCP socket", error);
                }
            }

            // Start listening again!
            startTcpAccept();
        });
    }

    inline void ConnectionWarning::receiveUdp(const asio::error_code &error) {
        if (!error || error == asio::error::message_size) {
            std::ostringstream os;
            os << "Got UDP connection attempt";
            addEndpointInfoToStream(os, m_udpRemoteEndpoint);
            std::cout << "***" << os.str() << std::endl;
            m_attempts.emplace_back(os.str());
        }
        startUdpReceive();
    }

} // namespace server
} // namespace osvr