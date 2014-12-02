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

#ifndef INCLUDED_Parameters_h_GUID_382472FF_8ED5_429E_6D51_0948F2F403D2
#define INCLUDED_Parameters_h_GUID_382472FF_8ED5_429E_6D51_0948F2F403D2

// Internal Includes
#include <osvr/ClientKit/ParametersC.h>

// Library/third-party includes
#include <boost/scoped_array.hpp>

// Standard includes
#include <string>

namespace osvr {
namespace clientkit {
    /// @brief Get a string parameter value from the given path.
    /// @param ctx Client context
    /// @param path A resource path (null-terminated string)
    /// @returns parameter value, or empty string if parameter does not exist or
    /// is not a string.
    inline std::string getStringParameter(OSVR_ClientContext ctx,
                                          const char path[]) {
        size_t len;
        osvrClientGetStringParameterLength(ctx, path, &len);
        std::string ret;
        if (len == 0) {
            return ret;
        }
        boost::scoped_array<char> buf(new char[len]);
        osvrClientGetStringParameter(ctx, path, buf.get());
        ret.assign(buf.get());
        return ret;
    }
} // namespace clientkit
} // namespace osvr

#endif // INCLUDED_Parameters_h_GUID_382472FF_8ED5_429E_6D51_0948F2F403D2
