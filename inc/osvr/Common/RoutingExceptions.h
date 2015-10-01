/** @file
    @brief Header

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

#ifndef INCLUDED_RoutingExceptions_h_GUID_DDBD7E30_8C15_46AE_4221_1EB366EA079D
#define INCLUDED_RoutingExceptions_h_GUID_DDBD7E30_8C15_46AE_4221_1EB366EA079D

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <stdexcept>
#include <string>

namespace osvr {
namespace common {
    /// @brief Exceptions that may be thrown from this module.
    ///
    /// @ingroup Routing
    namespace exceptions {
        /// @brief Thrown when attempting to add an invalid device name to the
        /// tree.
        struct InvalidDeviceName : std::runtime_error {
            InvalidDeviceName(std::string const &details)
                : std::runtime_error(
                      "Attempted to add invalid device name to tree: " +
                      details) {}
        };

        /// @brief Thrown when attempting to use a path with an empty component
        struct EmptyPathComponent : std::runtime_error {
            EmptyPathComponent(std::string const &path)
                : std::runtime_error(
                      "Cannot use a path with an empty component: " + path) {}
        };

        /// @brief Thrown when attempting to go to the parent path from the root
        struct ImpossibleParentPath : std::runtime_error {
            ImpossibleParentPath()
                : std::runtime_error("Cannot specify .. for parent path when "
                                     "already at the root!") {}
        };

        /// @brief Thrown when attempting to go use an absolute path when
        /// forbidden
        struct ForbiddenAbsolutePath : std::runtime_error {
            ForbiddenAbsolutePath()
                : std::runtime_error(
                      "Cannot specify an absolute path in this context!") {}
        };

        /// @brief Thrown when attempting to go to the parent path when
        /// forbidden
        struct ForbiddenParentPath : std::runtime_error {
            ForbiddenParentPath()
                : std::runtime_error("Cannot specify .. in a parent path used "
                                     "in this context!") {}
        };

        /// @brief Thrown when attempting to use an empty path
        struct EmptyPath : std::runtime_error {
            EmptyPath()
                : std::runtime_error("Cannot retrieve an empty path!") {}
        };

        /// @brief Thrown when attempting to use a path with no leading slash
        /// where an absolute path is required.
        struct PathNotAbsolute : std::runtime_error {
            PathNotAbsolute(std::string const &path)
                : std::runtime_error(
                      "Provided path was not absolute (no leading slash): " +
                      path) {}
        };

        /// @brief Contains exceptions thrown when, in the course of operation,
        /// invariants of the path tree are determined to have been violated.
        /// All inherit from InvariantError.
        namespace invariants {
            struct InvariantError : std::runtime_error {
                InvariantError(std::string const &msg)
                    : std::runtime_error("Path tree violated invariant: " +
                                         msg) {}
            };

            struct SensorMissingParent : InvariantError {
                SensorMissingParent(std::string const &path = std::string())
                    : InvariantError("Sensor element missing a parent! " +
                                     path) {}
            };
            struct SensorMissingInterfaceParent : InvariantError {
                SensorMissingInterfaceParent(
                    std::string const &path = std::string())
                    : InvariantError("Sensor element does not have an "
                                     "InterfaceElement parent! " +
                                     path) {}
            };

            struct InterfaceMissingParent : InvariantError {
                InterfaceMissingParent(std::string const &path = std::string())
                    : InvariantError("Interface elements must have a "
                                     "parent of some device type! " +
                                     path) {}
            };
        } // namespace invariants

    } // namespace exceptions
} // namespace common
} // namespace osvr
#endif // INCLUDED_RoutingExceptions_h_GUID_DDBD7E30_8C15_46AE_4221_1EB366EA079D
