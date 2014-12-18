/** @file
    @brief Header containing the class declaration for Interface, but not its
   inline implementation.

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

#ifndef INCLUDED_Interface_decl_h_GUID_8A07B1E7_4F57_4CA7_6BA8_3A262F486AB5
#define INCLUDED_Interface_decl_h_GUID_8A07B1E7_4F57_4CA7_6BA8_3A262F486AB5

// Internal Includes
#include <osvr/Util/ClientCallbackTypesC.h>
#include <osvr/Util/ClientOpaqueTypesC.h>

// Library/third-party includes

// Standard includes
// - none

namespace osvr {

namespace clientkit {

    class ClientContext;

    /// @brief Interface handle object. Typically acquired from a ClientContext.
    ///
    /// May be freely passed around and copied, as it does not confer ownership
    /// in the underlying interface object - that is maintained by the
    /// ClientContext. You may request, however, that ClientContext destroy the
    /// underlying interface object before it normally would (at context
    /// destruction).
    /// @ingroup ClientKitCPP
    class Interface {
      public:
        /// @brief Constructs an Interface object from an OSVR_ClientInterface
        /// object.
        Interface(ClientContext &ctx, OSVR_ClientInterface iface);

#define OSVR_CALLBACK_METHODS(TYPE)                                            \
    void registerCallback(OSVR_##TYPE##Callback cb, void *userdata);

        /// @name Callback registration methods
        /// @{
        OSVR_CALLBACK_METHODS(Pose)
        OSVR_CALLBACK_METHODS(Position)
        OSVR_CALLBACK_METHODS(Orientation)
        OSVR_CALLBACK_METHODS(Button)
        OSVR_CALLBACK_METHODS(Analog)
/// @}

#undef OSVR_CALLBACK_METHODS

        /// @brief Get the raw OSVR_ClientInterface from this wrapper.
        OSVR_ClientInterface get();

        /// @brief Manually free the interface before the context is closed.
        ///
        /// This is not required, but can be used, for instance, to ensure that
        /// a callback is not called with a reference to an already-deleted
        /// object.
        ///
        /// This will make use of this and any other copies of this Interface
        /// object illegal!
        ///
        /// @throws std::logic_error if the interface is null or already freed.
        void free();

      private:
        ClientContext *m_ctx;
        OSVR_ClientInterface m_interface;
    };
} // end namespace clientkit

} // end namespace osvr

#endif // INCLUDED_Interface_decl_h_GUID_8A07B1E7_4F57_4CA7_6BA8_3A262F486AB5
