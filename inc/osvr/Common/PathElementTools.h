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
#include <osvr/Common/Export.h>
#include <osvr/Common/PathElementTypes.h>

// Library/third-party includes
// - none

// Standard includes
#include <stddef.h>

namespace osvr {
namespace common {

#ifndef OSVR_DOXYGEN_EXTERNAL
    namespace detail {
        struct AliasPriorityWrapper {
            AliasPriority priority;
        };
        template <typename T>
        T &&operator<<(T &&os, AliasPriorityWrapper const &wrapper) {
            switch (wrapper.priority) {
            case ALIASPRIORITY_MINIMUM:
                os << "Minimum (" << int(ALIASPRIORITY_MINIMUM) << ")";
                break;

            case ALIASPRIORITY_AUTOMATIC:
                os << "Automatic (" << int(ALIASPRIORITY_AUTOMATIC) << ")";
                break;
            case ALIASPRIORITY_SEMANTICROUTE:
                os << "Semantic Route (" << int(ALIASPRIORITY_SEMANTICROUTE)
                   << ")";
                break;
            case ALIASPRIORITY_MANUAL:
                os << "Manual/Max (" << int(ALIASPRIORITY_MANUAL) << ")";
                break;
            default:
                os << int(ALIASPRIORITY_MANUAL);
                break;
            }
            return os;
        }
    } // namespace detail
#endif

    /// @brief Helper method to output a priority in a formatted way to a
    /// stream.
    inline detail::AliasPriorityWrapper outputPriority(AliasPriority priority) {
        return detail::AliasPriorityWrapper{priority};
    }

    namespace elements {
        /// @brief Gets a string that indicates the type of path element. Do not
        /// use this for conditionals/comparisons unless there's really no
        /// better way! (There probably is a better way with a variant
        /// static_visitor)
        /// @param elt The element to investigate.
        OSVR_COMMON_EXPORT const char *getTypeName(PathElement const &elt);

        /// @brief Gets the string that represents the templated type
        template <typename ElementType> inline const char *getTypeName() {
            return getTypeName(ElementType());
        }

        /// @brief If dest is a NullElement, replace it with the provided src
        /// element.
        /// @param dest Item to inquire about, and update if needed.
        /// @param src Replacement for dest if dest is a NullElement.
        void ifNullReplaceWith(PathElement &dest, PathElement const &src);

        /// @brief Returns true if the path element provided is a NullElement.
        bool isNull(PathElement const &elt);

        /// @brief Gets the length of the longest type name
        OSVR_COMMON_EXPORT size_t getMaxTypeNameLength();
    } // namespace elements
    using elements::getTypeName;
} // namespace common
} // namespace osvr
#endif // INCLUDED_PathElementTools_h_GUID_9246E2D5_1598_409F_BD30_1817FA2C1FB2
