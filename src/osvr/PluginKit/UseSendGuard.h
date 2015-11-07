/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_UseSendGuard_h_GUID_FEAB5647_E86B_4BA2_0A29_CB5665678CCB
#define INCLUDED_UseSendGuard_h_GUID_FEAB5647_E86B_4BA2_0A29_CB5665678CCB

// Internal Includes
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
// - none

// Standard includes
#include <exception>

/// Calls a function using the send guard, returning the return value of the
/// function if it completes without exception.
template <typename InterfaceType, typename F>
inline OSVR_ReturnCode useSendGuard(InterfaceType &iface, F &&func) {
    try {
        auto guard = iface->getSendGuard();
        if (guard->lock()) {
            return func();
        }
    } catch (std::exception const &e) {
        OSVR_DEV_VERBOSE("Caught exception: " << e.what());
        return OSVR_RETURN_FAILURE;
    } catch (...) {
        OSVR_DEV_VERBOSE("Caught non-standard exception!");
        return OSVR_RETURN_FAILURE;
    }
    return OSVR_RETURN_FAILURE;
}

/// Calls a void function using the send guard, returning success if it
/// completes without exception.
template <typename InterfaceType, typename F>
inline OSVR_ReturnCode useSendGuardVoid(InterfaceType &iface, F &&func) {
    return useSendGuard(iface, [&func]() -> OSVR_ReturnCode {
        func();
        return OSVR_RETURN_SUCCESS;
    });
}
#endif // INCLUDED_UseSendGuard_h_GUID_FEAB5647_E86B_4BA2_0A29_CB5665678CCB
