/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_GuardInterface_h_GUID_95F5DE8D_6ED0_4A80_9D86_EC8589CB2196
#define INCLUDED_GuardInterface_h_GUID_95F5DE8D_6ED0_4A80_9D86_EC8589CB2196

// Internal Includes
#include <osvr/Util/AnnotationMacrosC.h>
#include <osvr/Util/Export.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace util {
    /// @brief An interface to a (deferred) guard class: after instantiation and
    /// lock returning true, some resource is locked and available until
    /// destruction.
    class GuardInterface {
      public:
        OSVR_UTIL_EXPORT virtual ~GuardInterface();
        /// @brief Attempts to lock/request the resource, and returns true if
        /// successful.
        ///
        /// Only proceed with using the resource if this returns true!
        OSVR_RETURN_WARN_UNUSED virtual bool lock() = 0;
    };

} // namespace util
} // namespace osvr
#endif // INCLUDED_GuardInterface_h_GUID_95F5DE8D_6ED0_4A80_9D86_EC8589CB2196
