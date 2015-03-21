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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/Common/SystemComponent.h>
#include <osvr/Common/BaseDevice.h>
#include <osvr/Util/MessageKeys.h>
#include <osvr/Common/Serialization.h>
#include <osvr/Common/JSONSerializationTags.h>
#include <osvr/Common/Buffer.h>
#include <osvr/Common/PathTreeSerialization.h>

// Library/third-party includes
#include <json/value.h>

// Standard includes
// - none

namespace osvr {
namespace common {
    namespace messages {
        class RoutesFromServer::MessageSerialization {
          public:
            MessageSerialization(std::string const &str = std::string())
                : m_str(str) {}

            template <typename T> void processMessage(T &p) {
                p(m_str, serialization::StringOnlyMessageTag());
            }

          private:
            std::string m_str;
        };
        const char *RoutesFromServer::identifier() {
            return util::messagekeys::routingData();
        }

        const char *AppStartupToServer::identifier() {
            return "com.osvr.system.appstartup";
        }

        class ClientRouteToServer::MessageSerialization {
          public:
            MessageSerialization(std::string const &str = std::string())
                : m_str(str) {}

            template <typename T> void processMessage(T &p) {
                p(m_str, serialization::StringOnlyMessageTag());
            }

          private:
            std::string m_str;
        };
        const char *ClientRouteToServer::identifier() {
            return "com.osvr.system.updateroutetoserver";
        }

        class ConfigFromServer::MessageSerialization {
          public:
            MessageSerialization(Json::Value const &msg = Json::arrayValue)
                : m_msg(msg) {}

            template <typename T> void processMessage(T &p) {
                p(m_msg, serialization::JsonOnlyMessageTag());
            }

            Json::Value const &getValue() const { return m_msg; }

          private:
            Json::Value m_msg;
        };
        const char *ConfigFromServer::identifier() {
            return "com.osvr.system.configfromserver";
        }
    } // namespace messages

    const char *SystemComponent::deviceName() {
        return util::messagekeys::systemSender();
    }

    shared_ptr<SystemComponent> SystemComponent::create() {
        shared_ptr<SystemComponent> ret(new SystemComponent);
        return ret;
    }

    SystemComponent::SystemComponent() {}

    void SystemComponent::sendRoutes(std::string const &routes) {
        Buffer<> buf;
        messages::RoutesFromServer::MessageSerialization msg(routes);
        serialize(buf, msg);
        m_getParent().packMessage(buf, routesOut.getMessageType());
    }

    void SystemComponent::registerRoutesHandler(vrpn_MESSAGEHANDLER handler,
                                                void *userdata) {
        m_registerHandler(handler, userdata, routesOut.getMessageType());
    }

    void SystemComponent::sendClientRouteUpdate(std::string const &route) {
        Buffer<> buf;
        messages::ClientRouteToServer::MessageSerialization msg(route);
        serialize(buf, msg);
        m_getParent().packMessage(buf, routeIn.getMessageType());
    }

    void SystemComponent::registerClientRouteUpdateHandler(
        vrpn_MESSAGEHANDLER handler, void *userdata) {
        m_registerHandler(handler, userdata, routeIn.getMessageType());
    }
    void SystemComponent::sendReplacementTree(PathTree &tree) {
        Json::Value config(Json::arrayValue);
        {
            Json::Value cmd(Json::objectValue);
            cmd["method"] = "reset";
            config.append(cmd);
        }
        {
            Json::Value cmd(Json::objectValue);
            cmd["method"] = "add";
            cmd["params"] = pathTreeToJson(tree);
            config.append(cmd);
        }
        Buffer<> buf;
        messages::ConfigFromServer::MessageSerialization msg(config);
        serialize(buf, msg);
        m_getParent().packMessage(buf, configOut.getMessageType());

        m_getParent().sendPending(); // forcing this since it will cause
                                     // shuffling of remotes on the client.
    }
    void SystemComponent::registerAddToTreeHandler(JsonHandler cb) {
        m_registerConfigHandler();
        m_configAddToTreeHandlers.push_back(cb);
    }
    void SystemComponent::registerResetTreeHandler(ResetHandler cb) {
        m_registerConfigHandler();
        m_configResetHandlers.push_back(cb);
    }

    void SystemComponent::m_parentSet() {
        m_getParent().registerMessageType(routesOut);
        m_getParent().registerMessageType(appStartup);
        m_getParent().registerMessageType(routeIn);
        m_getParent().registerMessageType(configOut);
    }

    void SystemComponent::m_registerConfigHandler() {
        if (m_configAddToTreeHandlers.empty() &&
            m_configResetHandlers.empty()) {
            m_registerHandler(&SystemComponent::m_handleConfig, this,
                              configOut.getMessageType());
        }
    }
    int SystemComponent::m_handleConfig(void *userdata, vrpn_HANDLERPARAM p) {
        auto self = static_cast<SystemComponent *>(userdata);
        auto bufReader = readExternalBuffer(p.buffer, p.payload_len);
        messages::ConfigFromServer::MessageSerialization msg;
        deserialize(bufReader, msg);
        auto timestamp = util::time::fromStructTimeval(p.msg_time);
        BOOST_ASSERT_MSG(msg.getValue().isArray(),
                         "config message must be a JSON array");
        for (auto const &configCommand : msg.getValue()) {
            if (configCommand.isMember("method")) {
                auto method = configCommand["method"].asString();
                if (method == "reset") {
                    for (auto const &cb : self->m_configResetHandlers) {
                        cb(timestamp);
                    }
                } else if (method == "add") {
                    Json::Value nodes = configCommand["params"];
                    BOOST_ASSERT_MSG(nodes.isArray(),
                                     "params must be an array of nodes!");
                    for (auto const &cb : self->m_configAddToTreeHandlers) {
                        cb(nodes, timestamp);
                    }
                }
            }
        }
        return 0;
    }
} // namespace common
} // namespace osvr
