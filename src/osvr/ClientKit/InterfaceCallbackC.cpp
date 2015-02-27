/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/ClientKit/InterfaceCallbackC.h>
#include <osvr/Client/ClientInterface.h>

// Library/third-party includes
// - none

// Standard includes
// - none

#define OSVR_CALLBACK_METHODS(TYPE)                                            \
    OSVR_ReturnCode osvrRegister##TYPE##Callback(OSVR_ClientInterface iface,   \
                                                 OSVR_##TYPE##Callback cb,     \
                                                 void *userdata) {             \
        iface->registerCallback(cb, userdata);                                 \
        return OSVR_RETURN_SUCCESS;                                            \
    }

OSVR_CALLBACK_METHODS(Pose)
OSVR_CALLBACK_METHODS(Position)
OSVR_CALLBACK_METHODS(Orientation)
OSVR_CALLBACK_METHODS(Button)
OSVR_CALLBACK_METHODS(Analog)
OSVR_CALLBACK_METHODS(Imaging)

#undef OSVR_CALLBACK_METHODS