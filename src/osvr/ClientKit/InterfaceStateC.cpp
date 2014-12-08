/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/ClientKit/InterfaceStateC.h>
#include <osvr/Client/ClientInterface.h>

// Library/third-party includes
// - none

// Standard includes
// - none

#define OSVR_CALLBACK_METHODS(TYPE)                                            \
    OSVR_ReturnCode osvrGet##TYPE##State(OSVR_ClientInterface iface,           \
                                         struct OSVR_TimeValue *timestamp,     \
                                         OSVR_##TYPE##State *state) {          \
        bool hasState =                                                        \
            iface->getState<OSVR_##TYPE##Report>(*timestamp, *state);          \
        return hasState ? OSVR_RETURN_SUCCESS : OSVR_RETURN_FAILURE;           \
    }

OSVR_CALLBACK_METHODS(Pose)
OSVR_CALLBACK_METHODS(Position)
OSVR_CALLBACK_METHODS(Orientation)
OSVR_CALLBACK_METHODS(Button)
OSVR_CALLBACK_METHODS(Analog)

#undef OSVR_CALLBACK_METHODS