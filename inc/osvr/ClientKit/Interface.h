/** @file
    @brief Header containing the inline implementation of Interface

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

#ifndef INCLUDED_Interface_h_GUID_5D5B1FAD_AD72_4216_9FB6_6483D6EE7DF1
#define INCLUDED_Interface_h_GUID_5D5B1FAD_AD72_4216_9FB6_6483D6EE7DF1

// Internal Includes
#include <osvr/ClientKit/Interface_decl.h>
#include <osvr/ClientKit/Context_decl.h>
#include <osvr/ClientKit/InterfaceC.h>
#include <osvr/ClientKit/InterfaceCallbackC.h>

// Library/third-party includes
#include <boost/function.hpp>

// Standard includes
// - none

namespace osvr {

namespace clientkit {

    inline Interface::Interface(ClientContext &ctx, OSVR_ClientInterface iface)
        : m_ctx(&ctx), m_interface(iface) {}

    inline OSVR_ClientInterface Interface::get() { return m_interface; }

    inline void Interface::free() { m_ctx->free(*this); }

#define OSVR_CALLBACK_METHODS(TYPE)                                            \
    inline void Interface::registerCallback(OSVR_##TYPE##Callback cb,          \
                                            void *userdata) {                  \
        osvrRegister##TYPE##Callback(m_interface, cb, userdata);               \
    }

    OSVR_CALLBACK_METHODS(Pose)
    OSVR_CALLBACK_METHODS(Position)
    OSVR_CALLBACK_METHODS(Orientation)
    OSVR_CALLBACK_METHODS(Button)
    OSVR_CALLBACK_METHODS(Analog)

#undef OSVR_CALLBACK_METHODS

} // end namespace clientkit

} // end namespace osvr

#endif // INCLUDED_Interface_h_GUID_5D5B1FAD_AD72_4216_9FB6_6483D6EE7DF1
