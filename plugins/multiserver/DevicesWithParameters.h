/** @file
    @brief Header

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

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
