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
#include <osvr/Common/RouteContainer.h>
#include <osvr/Common/RoutingKeys.h>

// Library/third-party includes
#include <json/value.h>
#include <json/reader.h>
#include <json/writer.h>

// Standard includes
#include <stdexcept>
#include <functional>
#include <algorithm>

namespace osvr {
namespace common {

    static inline Json::Value
    parseRoutingDirective(std::string const &routingDirective) {
        Json::Reader reader;
        Json::Value val;
        if (!reader.parse(routingDirective, val)) {
            throw std::runtime_error("Invalid JSON routing directive: " +
                                     routingDirective);
        }
        return val;
    }

    static inline std::string
    getDestinationFromJson(Json::Value const &routingDirective) {
        return routingDirective.get(routing_keys::destination(), "").asString();
    }
    static inline std::string toFastString(Json::Value const &val) {
        Json::FastWriter writer;
        return writer.write(val);
    }

    static inline std::string
    getSourceFromJson(Json::Value const &routingDirective) {
        return routingDirective.get(routing_keys::source(), "")
            .toStyledString();
    }

    RouteContainer::RouteContainer() {}

    RouteContainer::RouteContainer(std::string const &routes) {
        Json::Reader reader;
        Json::Value routesVal;
        if (!reader.parse(routes, routesVal)) {
            throw std::runtime_error("Invalid JSON routing directive array: " +
                                     routes);
        }
        for (Json::ArrayIndex i = 0, e = routesVal.size(); i < e; ++i) {
            const Json::Value thisRoute = routesVal[i];
            m_addRoute(getDestinationFromJson(thisRoute),
                       toFastString(thisRoute));
        }
    }

    bool RouteContainer::addRoute(std::string const &routingDirective) {
        auto route = parseRoutingDirective(routingDirective);
        return m_addRoute(getDestinationFromJson(route), routingDirective);
    }

    std::string RouteContainer::getRoutes(bool styled) const {
        Json::Value routes(Json::arrayValue);
        for (auto const &r : m_routingDirectives) {
            routes.append(parseRoutingDirective(r));
        }
        if (styled) {
            return routes.toStyledString();
        }
        return toFastString(routes);
    }

    std::string
    RouteContainer::getSource(std::string const &destination) const {
        auto route = getRouteForDestination(destination);
        if (!route.empty()) {
            Json::Value directive = parseRoutingDirective(route);
            return getSourceFromJson(directive);
        }
        return std::string();
    }

    std::string RouteContainer::getSourceAt(size_t i) const {
        return getSourceFromJson(
            parseRoutingDirective(m_routingDirectives.at(i)));
    }

    std::string RouteContainer::getDestinationAt(size_t i) const {
        return getDestinationFromJson(
            parseRoutingDirective(m_routingDirectives.at(i)));
    }

    std::string RouteContainer::getRouteForDestination(
        std::string const &destination) const {
        auto it = std::find_if(
            begin(m_routingDirectives), end(m_routingDirectives),
            [&](std::string const &directive) {
                return (getDestinationFromJson(
                            parseRoutingDirective(directive)) == destination);
            });
        if (it != end(m_routingDirectives)) {
            return *it;
        }
        return std::string();
    }

    std::string
    RouteContainer::getDestinationFromString(std::string const &route) {
        return getDestinationFromJson(parseRoutingDirective(route));
    }
    std::string RouteContainer::getSourceFromString(std::string const &route) {
        return getSourceFromJson(parseRoutingDirective(route));
    }
    bool RouteContainer::m_addRoute(std::string const &destination,
                                    std::string const &routingDirective) {
        bool replaced = false;
        /// If a route already exists with the same destination, replace it
        /// with this new one.
        std::replace_if(
            begin(m_routingDirectives),
            end(m_routingDirectives), [&](std::string const &directive) {
                Json::Value candidate = parseRoutingDirective(directive);
                bool match = (getDestinationFromJson(candidate) == destination);
                if (match) {
                    replaced = true;
                }
                return match;
            }, routingDirective);

        /// If we didn't replace an existing route, just add this one.
        if (!replaced) {
            m_routingDirectives.push_back(routingDirective);
        }
        return !replaced;
    }
} // namespace common
} // namespace osvr
