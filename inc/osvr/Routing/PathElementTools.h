/** @file
    @brief Header

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_PathElementTools_h_GUID_9246E2D5_1598_409F_BD30_1817FA2C1FB2
#define INCLUDED_PathElementTools_h_GUID_9246E2D5_1598_409F_BD30_1817FA2C1FB2

// Internal Includes
#include <osvr/Routing/Export.h>
#include <osvr/Routing/PathElementTypes_fwd.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace routing {
    namespace elements {
#ifndef OSVR_DOXYGEN_EXTERNAL
        namespace detail {
            template <typename ElementType> struct ElementTypeName {
                OSVR_ROUTING_EXPORT static const char *get();
            };
        } // namespace detail
#endif
        /// @brief Gets a string that indicates the type of path element. Do not
        /// use this for conditionals/comparisons unless there's really no
        /// better way! (There probably is a better way with a variant
        /// static_visitor)
        /// @param elt The element to investigate.
        OSVR_ROUTING_EXPORT const char *getTypeName(PathElement const &elt);

        /// @brief Gets the string that represents the templated type
        template <typename ElementType> inline const char *getTypeName() {
            return detail::ElementTypeName<ElementType>::get();
        }

        /// @brief If dest is a NullElement, replace it with the provided src
        /// element.
        /// @param dest Item to inquire about, and update if needed.
        /// @param src Replacement for dest if dest is a NullElement.
        void ifNullReplaceWith(PathElement &dest, PathElement const &src);
    } // namespace elements
} // namespace routing
} // namespace osvr
#endif // INCLUDED_PathElementTools_h_GUID_9246E2D5_1598_409F_BD30_1817FA2C1FB2
