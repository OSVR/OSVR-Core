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

#ifndef INCLUDED_ConfigureServer_h_GUID_9D9EE94F_8F85_42BE_AFDD_4AA7B0EFA566
#define INCLUDED_ConfigureServer_h_GUID_9D9EE94F_8F85_42BE_AFDD_4AA7B0EFA566

// Internal Includes
#include <osvr/Server/Export.h>
#include <osvr/Server/ServerPtr.h>

// Library/third-party includes
// - none

// Standard includes
#include <string>
#include <iosfwd>

namespace osvr {
namespace server {
    /// @brief Creates a server, choosing the factory method according to the
    /// passed JSON configuration
    ///
    /// It first looks for an object under the `server` key - if not present, it
    /// will assume defaults for below. This object is the context for the below
    /// elements.
    ///
    /// If `interface` is defined, it will open a server on that interface (with
    /// an empty string meaning all interfaces)
    /// Otherwise, `local` is assumed to be true. If it is false, it will open
    /// on all interfaces.
    /// `port` defaults to the assigned VRPN port (3883)
    ///
    /// @throws std::runtime_error if parsing errors occur.
    /// @throws std::out_of_range if an invalid port (<1) is specified.
    OSVR_SERVER_EXPORT ServerPtr
    configuredConstruction(std::string const &json);

    /// @overload
    /// @param json An opened istream to read from.
    OSVR_SERVER_EXPORT ServerPtr configuredConstruction(std::istream &json);
} // namespace server
} // namespace osvr

#endif // INCLUDED_ConfigureServer_h_GUID_9D9EE94F_8F85_42BE_AFDD_4AA7B0EFA566
