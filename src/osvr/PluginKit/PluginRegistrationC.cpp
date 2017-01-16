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
#include <osvr/PluginKit/PluginRegistrationC.h>
#include "HandleNullContext.h"
#include <osvr/Util/Verbosity.h>
#include <osvr/PluginHost/PluginSpecificRegistrationContext.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

OSVR_ReturnCode osvrPluginRegisterHardwareDetectCallback(
    OSVR_INOUT_PTR OSVR_PluginRegContext ctx,
    OSVR_IN OSVR_HardwareDetectCallback detectCallback,
    OSVR_IN_OPT void *userData) {
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrPluginRegisterHardwareDetectCallback",
                                    ctx);

    try {
        osvr::pluginhost::PluginSpecificRegistrationContext::get(ctx)
            .registerHardwareDetectCallback(detectCallback, userData);
    } catch (std::exception &e) {
        std::cerr << "Error in osvrPluginRegisterHardwareDetectCallback - "
                     "caught exception reporting: " << e.what() << std::endl;
        return OSVR_RETURN_FAILURE;
    }
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrRegisterDriverInstantiationCallback(
    OSVR_INOUT_PTR OSVR_PluginRegContext ctx, OSVR_IN_STRZ const char *name,
    OSVR_IN_PTR OSVR_DriverInstantiationCallback cb,
    OSVR_IN_OPT void *userData) {
    try {
        osvr::pluginhost::PluginSpecificRegistrationContext::get(ctx)
            .registerDriverInstantiationCallback(name, cb, userData);
    } catch (std::exception &e) {
        std::cerr << "Error in osvrRegisterDriverInstantiationCallback - "
                     "caught exception reporting: " << e.what() << std::endl;
        return OSVR_RETURN_FAILURE;
    }
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrPluginRegisterDataWithDeleteCallback(
    OSVR_INOUT_PTR OSVR_PluginRegContext ctx,
    OSVR_IN OSVR_PluginDataDeleteCallback deleteCallback,
    OSVR_INOUT_PTR void *pluginData) {
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrPluginRegisterDataWithDeleteCallback",
                                    ctx);
    osvr::pluginhost::PluginSpecificRegistrationContext *context =
        static_cast<osvr::pluginhost::PluginSpecificRegistrationContext *>(ctx);
    context->registerDataWithDeleteCallback(deleteCallback, pluginData);
    return OSVR_RETURN_SUCCESS;
}

void osvrPluginLog(OSVR_INOUT_PTR OSVR_PluginRegContext ctx,
                   OSVR_IN OSVR_LogLevel severity,
                   OSVR_IN const char *message) {
    if (!ctx) {
        std::cerr << "[OSVR] " << message << std::endl;
        return;
    }

    osvr::pluginhost::PluginSpecificRegistrationContext *context =
        static_cast<osvr::pluginhost::PluginSpecificRegistrationContext *>(ctx);
    auto s = static_cast<osvr::util::log::LogLevel>(severity);
    context->log(s, message);
}

