/** @file
    @brief Header containing the class declaration for Interface, but not its
   inline implementation.

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

#ifndef INCLUDED_Interface_decl_h_GUID_8A07B1E7_4F57_4CA7_6BA8_3A262F486AB5
#define INCLUDED_Interface_decl_h_GUID_8A07B1E7_4F57_4CA7_6BA8_3A262F486AB5

// Internal Includes
#include <osvr/Util/ClientCallbackTypesC.h>
#include <osvr/Util/ClientOpaqueTypesC.h>
#include <osvr/Util/BoostDeletable.h>
#include <osvr/Util/ReportTypesX.h>

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

        /// @brief Empty constructor.
        Interface();

#define OSVR_X(TYPE)                                                           \
    void registerCallback(OSVR_##TYPE##Callback cb, void *userdata);

        /// @name Callback registration methods
        /// @{
        OSVR_INVOKE_REPORT_TYPES_XMACRO()
#undef OSVR_X
        /// @}

        /// @brief Determine if this interface object is empty (that is, was
        /// it once initialized). Does not determine if it has already been
        /// freed (see free())
        bool notEmpty() const;

        /// @brief Get the raw OSVR_ClientInterface from this wrapper.
        OSVR_ClientInterface get();

        /// @brief Get the associated ClientContext
        ClientContext &getContext();

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

        /// @brief Take (shared) ownership of some Deletable object.
        void takeOwnership(util::boost_util::DeletablePtr const &obj);

      private:
        ClientContext *m_ctx;
        OSVR_ClientInterface m_interface;
        util::boost_util::DeletableList m_deletables;
    };
} // end namespace clientkit

} // end namespace osvr

#endif // INCLUDED_Interface_decl_h_GUID_8A07B1E7_4F57_4CA7_6BA8_3A262F486AB5
