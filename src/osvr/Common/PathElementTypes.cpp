/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/Common/PathElementTypes.h>
#include <osvr/Util/DefaultPort.h>

// Library/third-party includes
// - none

// Standard includes
#include <regex>
#include <sstream>

namespace osvr {
namespace common {
    namespace elements {
        /// This chunk of code checks at compile time that:
        ///
        /// - All the types in our PathElement variant are derived from
        /// ElementBase<T>
        /// - For every type T in that variant, it is derived from
        /// ElementBase<T> - stronger than the check we put in the constructor,
        /// though it doesn't help with types that aren't listed in the variant,
        /// hence why we keep both checks.
        namespace {
            struct CRTPChecker {
                template <typename T> struct apply {
                    typedef ElementBase<T> base_type;
                    static_assert(std::is_base_of<base_type, T>::value,
                                  "A given element type T must inherit from "
                                  "ElementBase<T> (the CRTP)!");
                    typedef void type;
                };
            };

            // Force instantiation of the types for static asserts.
            typedef boost::mpl::transform<PathElement::types, CRTPChecker>::type
                CRTPCheckDummy;
        } // namespace

        AliasElement::AliasElement(std::string const &source,
                                   AliasPriority priority)
            : m_source(source), m_priority(priority) {}

        AliasElement::AliasElement(std::string const &source)
            : AliasElement(source, ALIASPRIORITY_MINIMUM) {}

        void AliasElement::setSource(std::string const &source) {
            /// @todo validation?
            m_source = source;
        }

        std::string &AliasElement::getSource() { return m_source; }
        std::string const &AliasElement::getSource() const { return m_source; }

        AliasPriority &AliasElement::priority() { return m_priority; }
        AliasPriority AliasElement::priority() const { return m_priority; }

        StringElement::StringElement() {}
        StringElement::StringElement(std::string const &s) : m_val(s) {}

        std::string &StringElement::getString() { return m_val; }

        std::string const &StringElement::getString() const { return m_val; }

        /// Helper function to augment a VRPN-style server specification with a
        /// port if one isn't already specified.
        static inline std::string
        attachPortToServerIfNoneSpecified(std::string const &server, int port) {
            auto endsWithPort = std::regex{".*:[[:digit:]]+$"};
            if (std::regex_match(server, endsWithPort)) {
                return server;
            }
            // OK, didn't end with a port
            if (server.find(":") != std::string::npos &&
                server.find("tcp://") == std::string::npos) {
                // Ah, but it's got a weird protocol specification or something,
                // more complicated than sticking tcp:// on the front, so we
                // should just leave it alone.
                return server;
            }
            // Now, we may append the port.
            std::ostringstream os;
            os << server;
            os << ":";
            os << port;
            return os.str();
        }

        DeviceElement
        DeviceElement::createVRPNDeviceElement(std::string const &deviceName,
                                               std::string const &server) {
            DeviceElement ret;
            ret.m_devName = deviceName;
            // explicitly specify VRPN port
            ret.m_server =
                attachPortToServerIfNoneSpecified(server, util::DefaultVRPNPort);

            return ret;
        }

        DeviceElement
        DeviceElement::createDeviceElement(std::string const &deviceName,
                                           std::string const &server,
                                           int port) {
            DeviceElement ret;
            ret.m_devName = deviceName;
            switch (port) {
            case util::OmitAppendingPort:
                ret.m_server = server;
                break;
            case util::UseDefaultPort:
                // explicitly specify OSVR port
                ret.m_server = attachPortToServerIfNoneSpecified(
                    server, util::DefaultOSVRPort);
                break;
            default:
                // explicitly specify the user's port
                ret.m_server = attachPortToServerIfNoneSpecified(server, port);
                break;
            }
            return ret;
        }

        std::string &DeviceElement::getDeviceName() { return m_devName; }
        std::string const &DeviceElement::getDeviceName() const {
            return m_devName;
        }

        std::string &DeviceElement::getServer() { return m_server; }
        std::string const &DeviceElement::getServer() const { return m_server; }

        std::string DeviceElement::getFullDeviceName() const {
            return getDeviceName() + "@" + getServer();
        }
        Json::Value &DeviceElement::getDescriptor() { return m_descriptor; }
        Json::Value const &DeviceElement::getDescriptor() const {
            return m_descriptor;
        }
    } // namespace elements
} // namespace common
} // namespace osvr
