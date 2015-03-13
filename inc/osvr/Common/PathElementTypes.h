/** @file
    @brief Header

    @date 2014

    @todo enforce/check CRTP using boost static asserts.

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

#ifndef INCLUDED_PathElementTypes_h_GUID_5CC817E5_C7CB_45AE_399D_0B0D39579374
#define INCLUDED_PathElementTypes_h_GUID_5CC817E5_C7CB_45AE_399D_0B0D39579374

// Internal Includes
#include <osvr/Common/Export.h>
#include <osvr/Common/PathElementTypes_fwd.h>

// Library/third-party includes
#include <boost/variant/variant.hpp>

// Standard includes
#include <string>

namespace osvr {
namespace common {
    /// @brief Namespace for the various element types that may constitute a
    /// node in the path tree.
    ///
    /// @ingroup Routing
    namespace elements {
        /// @brief Base, using the CRTP, providing some basic functionality for
        /// path elements.
        template <typename Type> class ElementBase {
          public:
            typedef Type type;
            const char *getTypeName() const;
        };

        /// @brief The element type created when requesting a path that isn't
        /// yet in the tree.
        class NullElement : public ElementBase<NullElement> {
          public:
            NullElement() = default;
        };

        /// @brief The element type corresponding to a plugin
        class PluginElement : public ElementBase<PluginElement> {
          public:
            PluginElement() = default;
        };

        /// @brief The element type corresponding to a device, which implements
        /// 0 or more interfaces
        class DeviceElement : public ElementBase<DeviceElement> {
          public:
            DeviceElement() = default;
            DeviceElement(std::string const &deviceName,
                          std::string const &server)
                : m_devName(deviceName), m_server(server) {}

            OSVR_COMMON_EXPORT static DeviceElement
            createVRPNDeviceElement(std::string const &deviceName,
                                    std::string const &server);

            OSVR_COMMON_EXPORT std::string const &getDeviceName() const;
            OSVR_COMMON_EXPORT std::string const &getServer() const;
            OSVR_COMMON_EXPORT std::string getFullDeviceName() const;

          private:
            std::string m_devName;
            std::string m_server;
        };

        /// @brief The element type corresponding to an interface, which often
        /// may have one or more sensors
        class InterfaceElement : public ElementBase<InterfaceElement> {
          public:
            InterfaceElement() = default;
        };

        /// @brief The element type corresponding to a particular sensor of an
        /// interface
        class SensorElement : public ElementBase<SensorElement> {

          public:
            SensorElement() = default;
        };

        /// @brief The element type serving as a physical association of other
        /// elements.
        class PhysicalAssociationElement
            : public ElementBase<PhysicalAssociationElement> {};

        /// @brief The element type corresponding to a logical grouping.
        class LogicalElement : public ElementBase<LogicalElement> {};

        /// @brief The element type corresponding to a "shallow" path alias -
        /// does not alias children.
        class AliasElement : public ElementBase<LogicalElement> {
          public:
            /// @brief Constructor with source.
            OSVR_COMMON_EXPORT AliasElement(std::string const &source);
            /// @brief Sets the source of this alias
            /// @param source absolute path of the target, possibly wrapped in
            /// transforms.
            /// @todo support relative paths - either here or at a different
            /// level
            void setSource(std::string const &source);

            /// @brief Get the source of data for this alias
            OSVR_COMMON_EXPORT std::string const &getSource() const;

          private:
            std::string m_source;
        };

    } // namespace elements

} // namespace common
} // namespace osvr

#endif // INCLUDED_PathElementTypes_h_GUID_5CC817E5_C7CB_45AE_399D_0B0D39579374
