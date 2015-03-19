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

// Library/third-party includes
// - none

// Standard includes
// - none

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

        AliasElement::AliasElement(std::string const &source)
            : m_source(source), m_automatic(false) {}

        void AliasElement::setSource(std::string const &source) {
            /// @todo validation?
            m_source = source;
        }

        std::string &AliasElement::getSource() { return m_source; }
        std::string const &AliasElement::getSource() const { return m_source; }

        bool &AliasElement::getAutomatic() { return m_automatic; }
        bool AliasElement::getAutomatic() const { return m_automatic; }

        DeviceElement
        DeviceElement::createVRPNDeviceElement(std::string const &deviceName,
                                               std::string const &server) {
            DeviceElement ret;
            ret.m_devName = deviceName;
            ret.m_server = server;
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
    } // namespace elements
} // namespace common
} // namespace osvr
