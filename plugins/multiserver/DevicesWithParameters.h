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

#ifndef INCLUDED_DevicesWithParameters_h_GUID_45F0E8B4_E094_4AAF_7D19_0EF3AFB7A11C
#define INCLUDED_DevicesWithParameters_h_GUID_45F0E8B4_E094_4AAF_7D19_0EF3AFB7A11C

// Internal Includes
#include "VRPNMultiserver.h"

// Library/third-party includes
#include <osvr/Util/PluginCallbackTypesC.h>

// Standard includes
#include <iostream>

typedef void (*DriverConstructor)(VRPNMultiserverData &data,
                                  OSVR_PluginRegContext ctx,
                                  const char *params);

template <DriverConstructor F>
OSVR_ReturnCode wrappedConstructor(OSVR_PluginRegContext ctx,
                                   const char *params, void *userData) {
    VRPNMultiserverData *data = static_cast<VRPNMultiserverData *>(userData);
    try {
        F(*data, ctx, params);
        return OSVR_RETURN_SUCCESS;
    } catch (std::exception &e) {
        std::cerr << "\nERROR: " << e.what() << "\n" << std::endl;
        return OSVR_RETURN_FAILURE;
    } catch (...) {
        return OSVR_RETURN_FAILURE;
    }
}

void createYEI(VRPNMultiserverData &data, OSVR_PluginRegContext ctx,
               const char *params);

#endif // INCLUDED_DevicesWithParameters_h_GUID_45F0E8B4_E094_4AAF_7D19_0EF3AFB7A11C
