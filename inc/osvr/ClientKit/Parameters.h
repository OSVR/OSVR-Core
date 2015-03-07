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
        osvrClientGetStringParameter(ctx, path, buf.get(), len);
        ret.assign(buf.get());
        return ret;
    }
} // namespace clientkit
} // namespace osvr

#endif // INCLUDED_Parameters_h_GUID_382472FF_8ED5_429E_6D51_0948F2F403D2
