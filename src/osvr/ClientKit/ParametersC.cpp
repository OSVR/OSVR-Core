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
#include <osvr/ClientKit/ParametersC.h>
#include <osvr/Client/ClientContext.h>

// Library/third-party includes
// - none

// Standard includes
// - none

OSVR_ReturnCode osvrClientGetStringParameterLength(OSVR_ClientContext ctx,
                                                   const char path[],
                                                   size_t *len) {
    if (ctx == nullptr) {
        return OSVR_RETURN_FAILURE;
    }
    if (len == nullptr) {
        return OSVR_RETURN_FAILURE;
    }
    std::string val = ctx->getStringParameter(path);
    *len = val.empty() ? 0 : (val.size() + 1);
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrClientGetStringParameter(OSVR_ClientContext ctx,
                                             const char path[], char *buf,
                                             size_t len) {
    if (ctx == nullptr) {
        return OSVR_RETURN_FAILURE;
    }
    if (buf == nullptr) {
        return OSVR_RETURN_FAILURE;
    }

    std::string val = ctx->getStringParameter(path);
    if (val.size() + 1 > len) {
        /// buffer too small.
        return OSVR_RETURN_FAILURE;
    }
    val.copy(buf, val.size());
    buf[val.size()] = '\0';
    return OSVR_RETURN_SUCCESS;
}
