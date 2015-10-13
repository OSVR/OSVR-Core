/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/Client/CreateContext.h>
#include "PureClientContext.h"
#include "AnalysisClientContext.h"
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
// - none

// Standard includes
#include <cstring>

namespace osvr {
namespace client {
    common::ClientContext *createContext(const char appId[],
                                         const char host[]) {
        common::ClientContext *ret = nullptr;
        if (!appId || std::strlen(appId) == 0) {
            OSVR_DEV_VERBOSE("Could not create client context - null or empty "
                             "appId provided!");
            return ret;
        }
        ret = common::makeContext<PureClientContext>(appId, host);
        return ret;
    }

    common::ClientContext *
    createAnalysisClientContext(const char appId[], const char host[],
                                vrpn_ConnectionPtr const &conn) {
        common::ClientContext *ret = nullptr;
        if (!appId || !appId[0]) {
            OSVR_DEV_VERBOSE("Could not create analysis client context - null "
                             "or empty appId provided!");
            return ret;
        }

        if (!host || !host[0]) {
            OSVR_DEV_VERBOSE("Could not create analysis client context - null "
                             "or empty host provided!");
            return ret;
        }

        if (!conn) {
            OSVR_DEV_VERBOSE("Could not create analysis client context - null "
                             "connection provided!");
            return ret;
        }

        ret = common::makeContext<AnalysisClientContext>(appId, host, conn);
        return ret;
    }

} // namespace client
} // namespace osvr
