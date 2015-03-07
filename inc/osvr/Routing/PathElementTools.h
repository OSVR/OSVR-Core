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

#ifndef INCLUDED_PathElementTools_h_GUID_9246E2D5_1598_409F_BD30_1817FA2C1FB2
#define INCLUDED_PathElementTools_h_GUID_9246E2D5_1598_409F_BD30_1817FA2C1FB2

// Internal Includes
#include <osvr/Routing/Export.h>
#include <osvr/Routing/PathElementTypes.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace routing {
    namespace elements {
        /// @addtogroup Routing
        /// @{

        /// @brief Gets a string that indicates the type of path element. Do not
        /// use this for conditionals/comparisons unless there's really no
        /// better way! (There probably is a better way with a variant
        /// static_visitor)
        /// @param elt The element to investigate.
        OSVR_ROUTING_EXPORT const char *getTypeName(PathElement const &elt);

        /// @brief Gets the string that represents the templated type
        template <typename ElementType> inline const char *getTypeName() {
            return getTypeName(ElementType());
        }

        /// @brief If dest is a NullElement, replace it with the provided src
        /// element.
        /// @param dest Item to inquire about, and update if needed.
        /// @param src Replacement for dest if dest is a NullElement.
        void ifNullReplaceWith(PathElement &dest, PathElement const &src);
        /// @}
    } // namespace elements
} // namespace routing
} // namespace osvr
#endif // INCLUDED_PathElementTools_h_GUID_9246E2D5_1598_409F_BD30_1817FA2C1FB2
