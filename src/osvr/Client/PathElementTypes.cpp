/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/Client/PathElementTypes.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace client {
    namespace elements {
#define OSVR_DEFINE_NAME_STRING(CLASS)                                         \
    namespace names {                                                          \
        const char CLASS[] = #CLASS;                                           \
    }
        OSVR_DEFINE_NAME_STRING(NullElement);
        OSVR_DEFINE_NAME_STRING(PluginElement);
        OSVR_DEFINE_NAME_STRING(InterfaceElement);

#undef OSVR_DEFINE_NAME_STRING

    } // namespace elements
} // namespace client
} // namespace osvr