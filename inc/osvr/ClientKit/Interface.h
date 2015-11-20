/** @file
    @brief Header containing the inline implementation of Interface

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

#ifndef INCLUDED_Interface_h_GUID_5D5B1FAD_AD72_4216_9FB6_6483D6EE7DF1
#define INCLUDED_Interface_h_GUID_5D5B1FAD_AD72_4216_9FB6_6483D6EE7DF1

// Internal Includes
#include <osvr/ClientKit/Interface_decl.h>
#include <osvr/ClientKit/Context_decl.h>
#include <osvr/ClientKit/InterfaceC.h>
#include <osvr/ClientKit/InterfaceCallbackC.h>
#include <osvr/Util/ReportTypesX.h>

// Library/third-party includes
#include <boost/function.hpp>

// Standard includes
// - none

namespace osvr {

namespace clientkit {

    inline Interface::Interface(ClientContext &ctx, OSVR_ClientInterface iface)
        : m_ctx(&ctx), m_interface(iface) {}

    inline Interface::Interface() : m_ctx(NULL), m_interface(NULL) {}

    inline bool Interface::notEmpty() const {
        return m_ctx != NULL && m_interface != NULL;
    }

    inline OSVR_ClientInterface Interface::get() { return m_interface; }

    inline ClientContext &Interface::getContext() { return *m_ctx; }

    inline void Interface::free() {
        m_deletables.clear();
        m_ctx->free(*this);
        m_ctx = NULL;
        m_interface = NULL;
    }

    inline void
    Interface::takeOwnership(util::boost_util::DeletablePtr const &obj) {
        m_deletables.push_back(obj);
    }

#define OSVR_X(TYPE)                                                           \
    inline void Interface::registerCallback(OSVR_##TYPE##Callback cb,          \
                                            void *userdata) {                  \
        osvrRegister##TYPE##Callback(m_interface, cb, userdata);               \
    }
    OSVR_INVOKE_REPORT_TYPES_XMACRO()
#undef OSVR_X

} // end namespace clientkit

} // end namespace osvr

#endif // INCLUDED_Interface_h_GUID_5D5B1FAD_AD72_4216_9FB6_6483D6EE7DF1
