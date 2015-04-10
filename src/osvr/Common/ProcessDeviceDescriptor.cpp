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

#define OSVR_DEV_VERBOSE_DISABLE

// Internal Includes
#include <osvr/Common/ProcessDeviceDescriptor.h>
#include <osvr/Common/PathTree.h>
#include <osvr/Common/PathElementTools.h>
#include <osvr/Common/RoutingConstants.h>
#include <osvr/Util/Flag.h>
#include <osvr/Util/Verbosity.h>

#include "PathParseAndRetrieve.h"

// Library/third-party includes
#include <json/value.h>
#include <json/reader.h>
#include <boost/noncopyable.hpp>

// Standard includes
// - none

namespace osvr {
namespace common {
    static const char INTERFACES_KEY[] = "interfaces";

    static inline util::Flag
    processInterfacesFromDescriptor(PathNode &devNode,
                                    Json::Value const &desc) {
        util::Flag changed;
        if (!desc.isMember(INTERFACES_KEY)) {
            // No interfaces member
            return changed;
        }
        Json::Value const &ifaces = desc[INTERFACES_KEY];
        if (!ifaces.isObject()) {
            // Interfaces member isn't an object
            return changed;
        }
        for (auto const &iface : ifaces.getMemberNames()) {
            auto &ifaceNode = detail::treePathRetrieve(devNode, iface);
            if (elements::isNull(ifaceNode.value())) {
                ifaceNode.value() = elements::InterfaceElement();
                changed.set();
            }
        }
        return changed;
    }

    static const char TARGET_KEY[] = "$target";
    static const char SEMANTIC_KEY[] = "semantic";
    namespace {
        class SemanticRecursion : boost::noncopyable {
          public:
            SemanticRecursion(PathNode &devNode) : m_devNode(devNode) {}

            util::Flag operator()(Json::Value const &semanticObject) {
                m_recurse(semanticObject, SEMANTIC_KEY);
                return m_flag;
            }

          private:
            bool m_add(Json::Value const &currentLevel,
                       std::string const &relativeSemanticPath) {
                OSVR_DEV_VERBOSE("SemanticRecursion::m_add "
                                 << relativeSemanticPath);
                return addAliasFromSourceAndRelativeDest(
                    m_devNode, currentLevel.toStyledString(),
                    relativeSemanticPath);
            }
            void m_recurse(Json::Value const &currentLevel,
                           std::string const &relativeSemanticPath) {
                OSVR_DEV_VERBOSE("SemanticRecursion::m_recurse "
                                 << relativeSemanticPath);
                if (currentLevel.isString()) {
                    m_flag += m_add(currentLevel, relativeSemanticPath);
                    return;
                }
                if (currentLevel.isObject()) {
                    Json::Value target = currentLevel[TARGET_KEY];
                    if (!target.isNull()) {
                        m_flag += m_add(target, relativeSemanticPath);
                    }

                    for (auto const &memberName :
                         currentLevel.getMemberNames()) {
                        if (TARGET_KEY == memberName) {
                            continue;
                        }
                        m_recurse(currentLevel[memberName],
                                  relativeSemanticPath + getPathSeparator() +
                                      memberName);
                    }
                }
            }
            PathNode &m_devNode;
            util::Flag m_flag;
        };
    } // namespace

    static inline util::Flag
    processSemanticFromDescriptor(PathNode &devNode, Json::Value const &desc) {
        util::Flag changed;
        Json::Value const &sem = desc[SEMANTIC_KEY];
        if (!sem.isObject()) {
            // Semantic member isn't an object or isn't a member
            return changed;
        }
        SemanticRecursion f{devNode};
        changed += f(sem);
        return changed;
    }

    bool processDeviceDescriptorForPathTree(PathTree &tree,
                                            std::string const &deviceName,
                                            std::string const &jsonDescriptor) {
        std::string devName{deviceName};
        if (getPathSeparatorCharacter() == devName.at(0)) {
            // Leading slash, which we'll need to drop from the device name
            devName.erase(begin(devName));
        }
        util::Flag changed;

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
            changed.set();
        }

        /// @todo store descriptor string in the device node

        /// Parse JSON
        Json::Value descriptor;
        {
            Json::Reader reader;
            if (!reader.parse(jsonDescriptor, descriptor)) {
                /// @todo warn about failed descriptor parse?
                return changed.get();
            }
        }

        changed += processInterfacesFromDescriptor(devNode, descriptor);

        changed += processSemanticFromDescriptor(devNode, descriptor);

        return changed.get();
    }

} // namespace common
} // namespace osvr
