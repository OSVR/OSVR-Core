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
#include <boost/function.hpp>

// Standard includes
// - none

namespace osvr {

namespace clientkit {

    /// @brief Interface handle object. Typically acquired from a ClientContext.
    /// @ingroup ClientKitCPP
    class Interface {
      public:
        /// @brief Constructs an Interface object from an OSVR_ClientInterface
        /// object.
        Interface(OSVR_ClientInterface interface);

        /// @brief Register a callback for some report type.
        template <typename T> void registerCallback(T cb, void *userdata);

        /// @brief Get the raw OSVR_ClientInterface from this wrapper.
        OSVR_ClientInterface get();

      private:
        OSVR_ClientInterface m_interface;

#define OSVR_CALLBACK_METHODS(TYPE)                                            \
    static boost::function<OSVR_ReturnCode(                                    \
        OSVR_ClientInterface, OSVR_##TYPE##Callback cb, void *userdata)>       \
    getCallbackRegisterFunction(const OSVR_##TYPE##Callback &);

        OSVR_CALLBACK_METHODS(Pose)
        OSVR_CALLBACK_METHODS(Position)
        OSVR_CALLBACK_METHODS(Orientation)
        OSVR_CALLBACK_METHODS(Button)
        OSVR_CALLBACK_METHODS(Analog)

#undef OSVR_CALLBACK_METHODS
    };

    inline Interface::Interface(OSVR_ClientInterface interface)
        : m_interface(interface) {}

    template <typename T>
    inline void Interface::registerCallback(T cb, void *userdata) {
        getCallbackRegisterFunction(cb)(m_interface, cb, userdata);
    }

    inline OSVR_ClientInterface Interface::get() { return m_interface; }

#define OSVR_CALLBACK_METHODS(TYPE)                                            \
    inline boost::function<OSVR_ReturnCode(                                    \
        OSVR_ClientInterface, OSVR_##TYPE##Callback cb, void *userdata)>       \
    Interface::getCallbackRegisterFunction(const OSVR_##TYPE##Callback &) {    \
        return osvrRegister##TYPE##Callback;                                   \
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
