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
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_AliasProcessor_h_GUID_9C10FB5E_F8B9_44BB_BCF4_9E50FFBFA1C9
#define INCLUDED_AliasProcessor_h_GUID_9C10FB5E_F8B9_44BB_BCF4_9E50FFBFA1C9

// Internal Includes
#include <osvr/Common/Export.h>
#include <osvr/Common/PathElementTypes_fwd.h>
#include <osvr/Common/PathNode_fwd.h>

// Library/third-party includes
#include <json/value.h>

// Standard includes
// - none

namespace osvr {
namespace common {
    namespace detail {
        /// @brief Options struct for internal usage by AliasProcessor
        struct AliasProcessorOptions {
            bool permitRelativePath = false;
            bool permitRelativeSource = false;
            bool permitWildcard = false;
            AliasPriority defaultPriority = ALIASPRIORITY_AUTOMATIC;
        };
    } // namespace detail

    /// @brief Given a single JSON object, if it's an old-fashioned "routing
    /// directive", convert it to a normal "alias"
    ///
    /// @return Converted aliases, value unchanged if not a routing directive
    OSVR_COMMON_EXPORT Json::Value convertRouteToAlias(Json::Value const &val);

    /// @brief Given a path and a destination, combine them into a JSON alias
    /// description.
    ///
    /// @return A JSON object for your alias, or a null value if either input is
    /// invalid.
    OSVR_COMMON_EXPORT Json::Value
    createJSONAlias(std::string const &path, Json::Value const &destination);

    /// @brief Given a JSON object describing one or more aliases, set the
    /// priority of the alias(es).
    /// @return Updated description, or input if unchanged/not applicable.
    OSVR_COMMON_EXPORT Json::Value
        applyPriorityToAlias(Json::Value const &alias, AliasPriority priority);

    /// @brief A short-lived class for setting up options then processing alias
    /// directives to apply to a path tree.
    ///
    /// Can be used with the "chained methods" idiom (recommended): Create an
    /// instance (no need for a name) and immediately call option methods, if
    /// desired, then call one of the action methods.
    ///
    /// Can handle wildcards (*) in the destination, relative sources and
    /// paths, and internally-specified alias priorities. Some of these
    /// capabilities are off by default and should be enabled before calling
    /// this method.
    class AliasProcessor {
      public:
        /// @name Option methods
        /// @brief All these return a reference to the current object, so can be
        /// chained together.
        /// @{
        /// @brief Turn on permitRelativePath in a chained method.
        AliasProcessor &enableRelativePath() {
            m_opts.permitRelativePath = true;
            return *this;
        }

        /// @brief Turn on permitRelativeSource in a chained method.
        AliasProcessor &enableRelativeSource() {
            m_opts.permitRelativeSource = true;
            return *this;
        }

        /// @brief Turn on permitWildcard in a chained method.
        AliasProcessor &enableWildcard() {
            m_opts.permitWildcard = true;
            return *this;
        }

        /// @brief Set defaultPriority in a chained method.
        AliasProcessor &setDefaultPriority(AliasPriority prio) {
            m_opts.defaultPriority = prio;
            return *this;
        }
        /// @}

        /// @name Action methods
        /// @brief These methods are to be the end of a chain since they perform
        /// an action with the previously-set options.
        /// @{
        /// @brief Adds the given alias(es) to the tree.
        ///
        /// @param node The parent node for relative path resolution, often the
        /// root.
        /// @param val The parsed Json::Value containing aliases.
        /// @return true if changes were made
        OSVR_COMMON_EXPORT bool process(PathNode &node, Json::Value const &val);
        /// @}
      private:
        detail::AliasProcessorOptions m_opts;
    };
} // namespace common
} // namespace osvr
#endif // INCLUDED_AliasProcessor_h_GUID_9C10FB5E_F8B9_44BB_BCF4_9E50FFBFA1C9
