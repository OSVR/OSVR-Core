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
#include "VRPNContext.h"
#include "PureClientContext.h"
#include <osvr/Util/Verbosity.h>
#include <osvr/Common/GetEnvironmentVariable.h>

// Library/third-party includes
// - none

// Standard includes
#include <cstring>

static const char PATHTREE_ENV_VAR[] = "OSVR_PATHTREE";

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

        if (common::getEnvironmentVariable(PATHTREE_ENV_VAR).is_initialized()) {
            // that environment variable has something in it - turn on testing.
            OSVR_DEV_VERBOSE("Caution: creating experimental PureClientContext "
                             "using PathTree!");
            ret = new PureClientContext(appId, host);
        } else {
            ret = new VRPNContext(appId, host);
        }
        return ret;
    }

} // namespace client
} // namespace osvr
