/** @file
    @brief Header

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

#ifndef INCLUDED_ClientContext_decl_h_GUID_1EFFF79A_3D9F_4794_9F98_37010949F386
#define INCLUDED_ClientContext_decl_h_GUID_1EFFF79A_3D9F_4794_9F98_37010949F386

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
// - none

// Internal Includes
#include <osvr/ClientKit/ContextC.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>

namespace osvr {

namespace clientkit {
    // Forward declaration
    class Interface;

    /// @brief Client context object: Create and keep one in your application.
    /// Handles lifetime management and provides access to ClientKit
    /// functionality.
    /// @ingroup ClientKitCPP
    class ClientContext : private boost::noncopyable {
      public:
        /// @brief Initialize the library.
        /// @param applicationIdentifier A string identifying your application.
        /// Reverse DNS format strongly suggested.
        /// @param flags initialization options (reserved) - pass 0 for now.
        ClientContext(const char applicationIdentifier[], uint32_t flags = 0u);

        /// @brief Initialize the context with an existing context.
        /// @note The ClientContext class will take ownership of the context.
        ClientContext(OSVR_ClientContext context);

        /// @brief Destructor: Shutdown the library.
        ~ClientContext();

        /// @brief Updates the state of the context - call regularly in your
        /// mainloop.
        void update();

        /// @brief Get the interface associated with the given path.
        /// @param path A resource path.
        /// @returns The interface object.
        Interface getInterface(const std::string &path);

        /// @brief Get a string parameter value from the given path.
        /// @param path A resource path.
        /// @returns parameter value, or empty string if parameter does not
        /// exist or is not a string.
        std::string getStringParameter(const std::string &path);

        /// @brief Frees an interface before it would normally be freed (at
        /// context close).
        void free(Interface &iface);

        /// @brief Gets the raw context.
        OSVR_ClientContext get();

      private:
        OSVR_ClientContext m_context;
    };

} // end namespace clientkit

} // end namespace osvr
#endif // INCLUDED_ClientContext_decl_h_GUID_1EFFF79A_3D9F_4794_9F98_37010949F386
