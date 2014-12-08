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
#include <osvr/Util/ClientCallbackTypesC.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {

namespace clientkit {

    class Interface {
      public:
        /// @brief Constructs an Interface object from an OSVR_ClientInterface
        /// object.
        Interface(OSVR_ClientInterface interface);

        /// @brief Frees the underlying OSVR_ClientInterface.
        ~Interface();

        /// @brief Copy constructor.
        Interface(const Interface &other);

#define OSVR_CALLBACK_METHODS(TYPE)                                            \
    inline void register##TYPE##Callback(OSVR_##TYPE##Callback cb,             \
                                         void *userdata);

        /// @group Callback methods.
        //{
        OSVR_CALLBACK_METHODS(Pose)
        OSVR_CALLBACK_METHODS(Position)
        OSVR_CALLBACK_METHODS(Orientation)
        OSVR_CALLBACK_METHODS(Button)
        OSVR_CALLBACK_METHODS(Analog)
//}

#undef OSVR_CALLBACK_METHODS

      private:
        OSVR_ClientInterface m_interface;
    };

    inline Interface::Interface(OSVR_ClientInterface interface)
        : m_interface(interface) {
        // do nothing
    }

    inline Interface::~Interface() {
        // do nothing
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
    OSVR_CALLBACK_METHODS(Analog)

#undef OSVR_CALLBACK_METHODS

} // end namespace clientkit

} // end namespace osvr

#endif // INCLUDED_Interface_h_GUID_5D5B1FAD_AD72_4216_9FB6_6483D6EE7DF1
