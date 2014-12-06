/** @file
    @brief Header

    @date 2014

    @author
    Kevin M. Godby
    <kevin@godby.org>
    <http://sensics.com>

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
#include <osvr/ClientKit/InterfaceC.h>
#include <osvr/ClientKit/InterfaceCallbackC.h>
#include <osvr/ClientKit/InterfacePtr.h>
#include <osvr/Util/ClientCallbackTypesC.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {

namespace clientkit {

    class Interface {
      public:
        // TODO Interface();
        Interface(OSVR_ClientInterface interface);
        ~Interface();

#define OSVR_CALLBACK_METHODS(TYPE)                                            \
    inline void register##TYPE##Callback(OSVR_##TYPE##Callback cb,             \
                                         void *userdata);

        OSVR_CALLBACK_METHODS(Pose)
        OSVR_CALLBACK_METHODS(Position)
        OSVR_CALLBACK_METHODS(Orientation)
        OSVR_CALLBACK_METHODS(Button)

#undef OSVR_CALLBACK_METHODS

      private:
        OSVR_ClientInterface m_interface;
    };

    /* TODO
    inline Interface::Interface(OSVR_ClientContext context, const std::string&
    path)
    {
        // TODO throw exception instead of return code
        osvrClientGetInterface(OSVR_ClientContext ctx, const char path[],
                               OSVR_ClientInterface *iface);
    }
    */

    inline Interface::Interface(OSVR_ClientInterface interface)
        : m_interface(interface) {
        // do nothing
    }

    inline Interface::~Interface() {
        // do nothing
        // TODO delete m_interface?
    }

#define OSVR_CALLBACK_METHODS(TYPE)                                            \
    inline void Interface::register##TYPE##Callback(OSVR_##TYPE##Callback cb,  \
                                                    void *userdata) {          \
        osvrRegister##TYPE##Callback(m_interface, cb,                          \
                                     userdata); /* always returns SUCCESS */   \
    }

    OSVR_CALLBACK_METHODS(Pose)
    OSVR_CALLBACK_METHODS(Position)
    OSVR_CALLBACK_METHODS(Orientation)
    OSVR_CALLBACK_METHODS(Button)

#undef OSVR_CALLBACK_METHODS

} // end namespace clientkit

} // end namespace osvr

#endif // INCLUDED_Interface_h_GUID_5D5B1FAD_AD72_4216_9FB6_6483D6EE7DF1
