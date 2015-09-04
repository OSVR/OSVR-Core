/** @file
    @brief Header

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
        /// @param flags initialization options (reserved, optional)
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

        /// @brief Checks to see if the client context is properly and fully
        /// started up.
        ///
        /// If this returns false, there may not be a server running, or it may
        /// be taking longer than usual to connect. The result will not change
        /// from false to true without calling update() - consider a loop.
        bool checkStatus() const;

        /// @brief Gets the bare OSVR_ClientContext.
        OSVR_ClientContext get();

      private:
        OSVR_ClientContext m_context;
    };

} // end namespace clientkit

} // end namespace osvr
#endif // INCLUDED_ClientContext_decl_h_GUID_1EFFF79A_3D9F_4794_9F98_37010949F386
