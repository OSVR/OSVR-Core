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
#include <osvr/Common/ProcessDeviceDescriptor.h>
#include <osvr/Common/PathTree.h>
#include <osvr/Common/PathElementTools.h>
#include <osvr/Common/RoutingConstants.h>

#include "PathParseAndRetrieve.h"

// Library/third-party includes
#include <json/value.h>
#include <json/reader.h>

// Standard includes
// - none

namespace osvr {
namespace common {
    static const char INTERFACES_KEY[] = "interfaces";

    static inline bool
    processInterfacesFromDescriptor(PathNode &devNode,
                                    Json::Value const &desc) {
        bool ret = false;
        if (!desc.isMember(INTERFACES_KEY)) {
            // No interfaces member
            return ret;
        }
        Json::Value const &ifaces = desc[INTERFACES_KEY];
        if (!ifaces.isObject()) {
            // Interfaces member isn't an object
            return ret;
        }
        for (auto const &iface : ifaces.getMemberNames()) {
            auto &ifaceNode = detail::treePathRetrieve(iface, devNode);
            if (elements::isNull(ifaceNode.value())) {
                ifaceNode.value() = elements::InterfaceElement();
                ret = true;
            }
        }
        return ret;
    }

    bool processDeviceDescriptorForPathTree(PathTree &tree,
                                            std::string const &deviceName,
                                            std::string const &jsonDescriptor) {
        std::string devName{deviceName};
        if (getPathSeparatorCharacter() == devName.at(0)) {
            // Leading slash, which we'll need to drop from the device name
            devName.erase(begin(devName));
        }
        bool ret = false;

        /// Set up device node
        auto &devNode = tree.getNodeByPath(getPathSeparator() + devName);
        if (elements::isNull(devNode.value())) {
            std::string host{"localhost"};
            auto atLocation = devName.find('@');
            if (std::string::npos != atLocation) {
                // Split host from device
                host = devName.substr(atLocation + 1);
                devName.resize(atLocation);
            }
            devNode.value() =
                elements::DeviceElement::createVRPNDeviceElement(devName, host);
            ret = true;
        }

        /// @todo store descriptor string in the device node

        /// Parse JSON
        Json::Value descriptor;
        {
            Json::Reader reader;
            if (!reader.parse(jsonDescriptor, descriptor)) {
                /// @todo warn about failed descriptor parse?
                return ret;
            }
        }

        ret = processInterfacesFromDescriptor(devNode, descriptor) || ret;

        return ret;
    }

} // namespace common
} // namespace osvr
