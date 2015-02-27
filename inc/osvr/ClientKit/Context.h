/** @file
    @brief Header

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

#ifndef INCLUDED_Context_h_GUID_DD0155F5_61A4_4A76_8C2E_D9614C7A9EBD
#define INCLUDED_Context_h_GUID_DD0155F5_61A4_4A76_8C2E_D9614C7A9EBD

// Internal Includes
#include <osvr/ClientKit/Context_decl.h>
#include <osvr/ClientKit/ContextC.h>
#include <osvr/ClientKit/ParametersC.h>
#include <osvr/ClientKit/Interface.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>
#include <boost/scoped_array.hpp>

// Standard includes
#include <string>
#include <stdexcept>

namespace osvr {

namespace clientkit {
    inline ClientContext::ClientContext(const char applicationIdentifier[],
                                        uint32_t flags)
        : m_context(osvrClientInit(applicationIdentifier, flags)) {}

    inline ClientContext::ClientContext(OSVR_ClientContext context)
        : m_context(context) {}

    inline ClientContext::~ClientContext() { osvrClientShutdown(m_context); }

    inline void ClientContext::update() {
        OSVR_ReturnCode ret = osvrClientUpdate(m_context);
        if (OSVR_RETURN_SUCCESS != ret) {
            throw std::runtime_error("Error updating context.");
        }
    }

    inline Interface ClientContext::getInterface(const std::string &path) {
        OSVR_ClientInterface interface = NULL;
        OSVR_ReturnCode ret =
            osvrClientGetInterface(m_context, path.c_str(), &interface);
        if (OSVR_RETURN_SUCCESS != ret) {
            throw std::runtime_error(
                "Couldn't create interface because the path was invalid.");
        }

        return Interface(*this, interface);
    }

    inline std::string
    ClientContext::getStringParameter(const std::string &path) {
        size_t length = 0;
        OSVR_ReturnCode ret = osvrClientGetStringParameterLength(
            m_context, path.c_str(), &length);
        if (OSVR_RETURN_SUCCESS != ret) {
            throw std::runtime_error(
                "Invalid context or null reference to length variable.");
        }

        if (0 == length) {
            return std::string();
        }

        boost::scoped_array<char> buf(new char[length]);
        ret = osvrClientGetStringParameter(m_context, path.c_str(), buf.get(),
                                           length);
        if (OSVR_RETURN_SUCCESS != ret) {
            throw std::runtime_error("Invalid context, null reference to "
                                     "buffer, or buffer is too small.");
        }

        return std::string(buf.get(), length);
    }

    inline void ClientContext::free(Interface &iface) {
        OSVR_ReturnCode ret = osvrClientFreeInterface(m_context, iface.get());
        if (OSVR_RETURN_SUCCESS != ret) {
            throw std::logic_error(
                "Could not free interface: either null or already freed!");
        }
        // Null out the interface.
        iface = Interface(*this, NULL);
    }

    inline OSVR_ClientContext ClientContext::get() { return m_context; }

} // end namespace clientkit

} // end namespace osvr

#endif // INCLUDED_Context_h_GUID_DD0155F5_61A4_4A76_8C2E_D9614C7A9EBD
