/** @file
    @brief Header

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

#ifndef INCLUDED_RouteContainer_h_GUID_EA5F3FB5_13E4_4299_5B39_1C986BC8E784
#define INCLUDED_RouteContainer_h_GUID_EA5F3FB5_13E4_4299_5B39_1C986BC8E784

// Internal Includes
#include <osvr/Common/Export.h>

// Library/third-party includes
// - none

// Standard includes
#include <vector>
#include <string>

namespace osvr {
namespace common {
    class RouteContainer {
      public:
        /// @brief Empty constructor
        OSVR_COMMON_EXPORT RouteContainer();

        /// @brief Constructor from a JSON array of routes.
        OSVR_COMMON_EXPORT explicit RouteContainer(std::string const &routes);

        /// @brief Register a JSON string as a routing directive.
        /// @returns true if the route was new, false if it replaced a previous
        /// route for that destination.
        OSVR_COMMON_EXPORT bool addRoute(std::string const &routingDirective);

        /// @brief Get a JSON array of all routing directives.
        /// @param styled Pass `true` if you want the result pretty-printed.
        OSVR_COMMON_EXPORT std::string getRoutes(bool styled = false) const;

        /// @brief Get the source JSON string for a given destination path.
        /// @returns an empty string if the destination was not found.
        OSVR_COMMON_EXPORT std::string
        getSource(std::string const &destination) const;

        OSVR_COMMON_EXPORT std::string getSourceAt(size_t i) const;

        OSVR_COMMON_EXPORT std::string getDestinationAt(size_t i) const;

        /// @brief Get the full routing JSON string for a given destination
        /// path.
        /// @returns an empty string if the destination was not found.
        OSVR_COMMON_EXPORT std::string
        getRouteForDestination(std::string const &destination) const;

        /// @brief Gets the number of directives
        std::size_t size() const { return m_routingDirectives.size(); }

        std::vector<std::string> const &getRouteList() const {
            return m_routingDirectives;
        }

        /// @brief Gets the destination of a route, given the route in string
        /// format.
        OSVR_COMMON_EXPORT static std::string
        getDestinationFromString(std::string const &route);
        /// @brief Gets the source of a route, given the route in string
        /// format.
        OSVR_COMMON_EXPORT static std::string
        getSourceFromString(std::string const &route);

      private:
        /// @brief Internal add route helper function, for when we've already
        /// parsed the destination from the directive.
        /// @returns true if the route was new, false if it replaced a previous
        /// route for that destination.
        bool m_addRoute(std::string const &destination,
                        std::string const &directive);
        std::vector<std::string> m_routingDirectives;
    };
} // namespace common
} // namespace osvr
#endif // INCLUDED_RouteContainer_h_GUID_EA5F3FB5_13E4_4299_5B39_1C986BC8E784
