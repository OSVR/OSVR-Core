/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_InternalInterfaceOwner_h_GUID_CE168C67_29A8_44A4_44E4_BCE428AF71EB
#define INCLUDED_InternalInterfaceOwner_h_GUID_CE168C67_29A8_44A4_44E4_BCE428AF71EB

// Internal Includes
#include <osvr/Common/ClientContext.h>
#include <osvr/Common/ClientInterfacePtr.h>
#include <osvr/Util/ClientOpaqueTypesC.h>

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
#include <utility>
#include <algorithm>

namespace osvr {
namespace client {
    /// @brief Class that should be used for all internally-used client
    /// interface objects as it handles ownership with respect to the client
    /// context properly.
    class InternalInterfaceOwner {
      public:
        /// @brief Empty constructor
        InternalInterfaceOwner() { m_checkInvariants(); }

        /// @brief Constructor
        InternalInterfaceOwner(common::ClientContext *ctx,
                               OSVR_ClientInterface iface)
            : m_ctx(ctx), m_iface(iface) {
            m_checkInvariants();
        }

        /// @brief Constructor that keeps an additional reference to the
        /// interface.
        InternalInterfaceOwner(common::ClientContext *ctx,
                               common::ClientInterfacePtr const &iface)
            : m_ctx(ctx), m_iface(iface.get()), m_owningIface(iface) {
            m_checkInvariants();
        }

        /// @brief Constructor from a context and path
        InternalInterfaceOwner(common::ClientContext *ctx, const char path[])
            : m_ctx(ctx) {
            m_owningIface = ctx->getInterface(path);
            m_iface = m_owningIface.get();
            m_checkInvariants();
        }

        /// @brief non-copyable
        InternalInterfaceOwner(InternalInterfaceOwner const &) = delete;

        /// @brief non-assignable
        InternalInterfaceOwner &
        operator=(InternalInterfaceOwner const &) = delete;

        /// @brief move constructible
        InternalInterfaceOwner(InternalInterfaceOwner &&other)
            : m_ctx(std::move(other.m_ctx)),
              m_owningIface(std::move(other.m_owningIface)) {
            std::swap(m_iface, other.m_iface);
            m_checkInvariants();
        }

        /// @brief move-assignable
        InternalInterfaceOwner &operator=(InternalInterfaceOwner &&other) {
            if (&other == this) {
                return *this;
            }
            m_reset();
            std::swap(m_ctx, other.m_ctx);
            std::swap(m_iface, other.m_iface);
            m_owningIface = std::move(other.m_owningIface);
            m_checkInvariants();
            return *this;
        }

        typedef osvr::common::ClientInterface contained_type;
        typedef contained_type &reference;
        typedef contained_type *pointer;

        /// @brief Check to see if valid
        explicit operator bool() const { return nullptr != m_iface; }

        /// @name Act like a pointer
        /// @{
        reference operator*() const { return *m_iface; }
        pointer operator->() const { return m_iface; }
        ///@}

        /// @brief Destructor - frees interface.
        ~InternalInterfaceOwner() { m_reset(); }

      private:
        void m_reset() {
            if (m_ctx && m_iface) {
                auto oldIface = m_ctx->releaseInterface(m_iface);
                BOOST_ASSERT_MSG(oldIface, "We should never be the last one "
                                           "holding a reference to this "
                                           "interface!");
                m_ctx = nullptr;
                m_iface = nullptr;
                m_owningIface.reset();
                oldIface.reset();
            }
        }

        void m_checkInvariants() const {
            BOOST_ASSERT_MSG(
                (!m_iface || (m_iface && m_ctx)),
                "If we have an interface, we must have a context.");
            BOOST_ASSERT_MSG(!m_owningIface || (m_owningIface.get() == m_iface),
                             "Either owningIface should be empty or the same "
                             "as our raw pointer.");
        }
        common::ClientContext *m_ctx = nullptr;
        osvr::common::ClientInterface *m_iface = nullptr;
        osvr::common::ClientInterfacePtr m_owningIface;
    };

} // namespace client
} // namespace osvr

#endif // INCLUDED_InternalInterfaceOwner_h_GUID_CE168C67_29A8_44A4_44E4_BCE428AF71EB
