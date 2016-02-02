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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/Common/ProcessArticulationSpec.h>
#include <osvr/Common/PathTree.h>
#include <osvr/Common/PathNode.h>
#include <osvr/Common/PathElementTools.h>
#include <osvr/Common/RoutingConstants.h>
#include <osvr/Util/Flag.h>
#include <osvr/Util/Verbosity.h>
#include <osvr/Util/TreeTraversalVisitor.h>
#include <osvr/Common/AliasProcessor.h>
#include <osvr/Common/NormalizeDeviceDescriptor.h>

// Library/third-party includes
#include <json/value.h>
#include <json/reader.h>
#include <boost/noncopyable.hpp>
#include <boost/variant/get.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/erase.hpp>

// Standard includes
// - none

namespace osvr {
namespace common {

    static const char ARTICULATION_KEY[] = "articulationSpec";
    static const char DATA_KEY[] = "$data";
    namespace {

        class ArticulationsRecursion : boost::noncopyable {
          public:
            ArticulationsRecursion(PathNode &devNode) : m_devNode(devNode) {}

            void operator()(Json::Value const &articulationsObject) {
                m_recurse(articulationsObject, ARTICULATION_KEY);
            }

          private:
            void m_add(Json::Value const &currentLevel,
                       std::string const &relativeArticulationsPath) {
                addArticulation(m_devNode, currentLevel.toStyledString(),
                                relativeArticulationsPath);
            }
            void m_recurse(Json::Value const &currentLevel,
                           std::string const &relativeArticulationsPath) {
                // from the top we go iterate the array
                // if (currentLevel.isArray()) {
                //    for (auto const &obj : currentLevel) {
                //        m_recurse(obj, relativeArticulationsPath);
                //    }
                //}
                if (currentLevel.isObject()) {
                    Json::Value data = currentLevel[DATA_KEY];
                    if (!data.isNull()) {
                        m_add(data, relativeArticulationsPath);
                    }

                    for (auto const &memberName :
                         currentLevel.getMemberNames()) {
                        if (DATA_KEY == memberName) {
                            continue;
                        }
                        m_recurse(currentLevel[memberName],
                                  relativeArticulationsPath +
                                      getPathSeparator() + memberName);
                    }
                }
            }
            PathNode &m_devNode;
        };

    } // namespace

    void processArticulationSpec(PathNode &devNode, Json::Value const &desc) {
        Json::Value const &articulations = desc;
        if (!articulations.isObject()) {
            // Articulation spec member isn't an object
            return;
        }
        ArticulationsRecursion f{devNode};
        f(articulations);
        return;
    }

    void processArticulationSpecForPathTree(PathTree &tree,
                                            std::string const &deviceName,
                                            Json::Value const &articSpec) {

        std::string devName = std::string{deviceName};
        std::string host{"localhost"};
        auto atLocation = deviceName.find('@');
        if (std::string::npos != atLocation) {
            // Split host from device
            host = devName.substr(atLocation + 1);
            devName.resize(atLocation);
        }

        if (osvr::common::getPathSeparatorCharacter() == devName.at(0)) {
            // Leading slash, which we'll need to drop from the device name
            devName.erase(begin(devName));
        }
        auto &devNode =
            tree.getNodeByPath(osvr::common::getPathSeparator() + devName);
        auto devElt =
            boost::get<osvr::common::elements::DeviceElement>(&devNode.value());
        if (nullptr == devElt) {
            devNode.value() =
                osvr::common::elements::DeviceElement::createVRPNDeviceElement(
                    devName, host);
            devElt = boost::get<osvr::common::elements::DeviceElement>(
                &devNode.value());
        }

        processArticulationSpec(devNode, articSpec);
    }

} // namespace common
} // namespace osvr
