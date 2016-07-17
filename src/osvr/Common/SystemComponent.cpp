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
#include <osvr/Common/CommonComponent.h>

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

        class ReplacementTreeFromServer::MessageSerialization {
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
        const char *ReplacementTreeFromServer::identifier() {
            return "com.osvr.system.ReplacementTreeFromServer";
        }

        class RegisteredStringMapRecord::MessageSerialization {
          public:
            explicit MessageSerialization(RegisteredStringMap const &myMap)
                : m_data(myMap.getEntries()) {}

            MessageSerialization() = default;

            template <typename T> void processMessage(T &p) { p(m_data); }
            GestureMapData const &getData() const { return m_data; }

          private:
            GestureMapData m_data;
        };
        const char *RegisteredStringMapRecord::identifier() {
            return "com.osvr.system.regstringmaprecord";
        }
    } // namespace messages

    const char *SystemComponent::deviceName() {
        return util::messagekeys::systemSender();
    }

    shared_ptr<SystemComponent> SystemComponent::create() {
        shared_ptr<SystemComponent> ret(new SystemComponent);
        return ret;
    }

    SystemComponent::SystemComponent()
        : m_nameToIDMap(make_shared<RegStringMapData>()) {}

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
        auto config = pathTreeToJson(tree);
        Buffer<> buf;
        messages::ReplacementTreeFromServer::MessageSerialization msg(config);
        serialize(buf, msg);
        m_getParent().packMessage(buf, treeOut.getMessageType());

        m_getParent().sendPending(); // forcing this since it will cause
                                     // shuffling of remotes on the client.
    }
    void SystemComponent::registerReplaceTreeHandler(JsonHandler const &cb) {
        if (m_replaceTreeHandlers.empty()) {
            m_registerHandler(&SystemComponent::m_handleReplaceTree, this,
                              treeOut.getMessageType());
        }
        m_replaceTreeHandlers.push_back(cb);
    }

    GestureDataPtr SystemComponent::getGestureMap() { return m_nameToIDMap; }

    void SystemComponent::sendGestureMap() {
        m_nameToIDMap->map.clearModifiedFlag();
        Buffer<> buf;
        auto msg = messages::RegisteredStringMapRecord::MessageSerialization{
            m_nameToIDMap->map};
        serialize(buf, msg);
        m_getParent().packMessage(buf, gestureStringMap.getMessageType());
    }

    void
    SystemComponent::registerGestureMapHandler(GestureMapHandler const &cb) {
        if (m_cb_map.empty()) {
            m_registerHandler(&SystemComponent::m_handleRegStringMap, this,
                              gestureStringMap.getMessageType());
        }
        m_cb_map.push_back(cb);
    }

    void SystemComponent::m_parentSet() {

        // add a ping handler to re-send string to ID map everytime the new
        // connection(ping) occurs
        m_commonComponent =
            m_getParent().addComponent(osvr::common::CommonComponent::create());

        m_getParent().registerMessageType(routesOut);
        m_getParent().registerMessageType(appStartup);
        m_getParent().registerMessageType(routeIn);
        m_getParent().registerMessageType(treeOut);
        m_getParent().registerMessageType(gestureStringMap);
        m_commonComponent->registerPingHandler([&] { sendGestureMap(); });
    }

    int SystemComponent::m_handleReplaceTree(void *userdata,
                                             vrpn_HANDLERPARAM p) {
        auto self = static_cast<SystemComponent *>(userdata);
        auto bufReader = readExternalBuffer(p.buffer, p.payload_len);
        messages::ReplacementTreeFromServer::MessageSerialization msg;
        deserialize(bufReader, msg);
        auto timestamp = util::time::fromStructTimeval(p.msg_time);
        BOOST_ASSERT_MSG(msg.getValue().isArray(),
                         "replace tree message must be an array of nodes!");
        for (auto const &cb : self->m_replaceTreeHandlers) {
            cb(msg.getValue(), timestamp);
        }
        return 0;
    }

    int VRPN_CALLBACK
    SystemComponent::m_handleRegStringMap(void *userdata, vrpn_HANDLERPARAM p) {
        auto self = static_cast<SystemComponent *>(userdata);
        auto bufReader = readExternalBuffer(p.buffer, p.payload_len);
        messages::RegisteredStringMapRecord::MessageSerialization msg;
        deserialize(bufReader, msg);
        auto data = msg.getData();

        for (auto const &cb : self->m_cb_map) {
            cb(data);
        }
        return 0;
    }

} // namespace common
} // namespace osvr
