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

#ifndef INCLUDED_ContextImpl_h_GUID_9000C62E_3693_4888_83A2_0D26F4591B6A
#define INCLUDED_ContextImpl_h_GUID_9000C62E_3693_4888_83A2_0D26F4591B6A

// Internal Includes
#include <osvr/Client/Export.h>
#include <osvr/Client/ClientContext_fwd.h>
#include <osvr/Client/ClientInterfacePtr.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>
#include <vector>
#include <map>

struct OSVR_ClientContextObject : boost::noncopyable {
  public:
    typedef std::vector<::osvr::client::ClientInterfacePtr> InterfaceList;
    /// @brief Destructor
    virtual ~OSVR_ClientContextObject();

    /// @brief System-wide update method.
    OSVR_CLIENT_EXPORT void update();

    /// @brief Accessor for app ID
    std::string const &getAppId() const;

    /// @brief Creates an interface object for the given path. The context
    /// retains shared ownership.
    ///
    /// @param path Path to a resource. Should be absolute.
    OSVR_CLIENT_EXPORT::osvr::client::ClientInterfacePtr
    getInterface(const char path[]);

    /// @brief Searches through this context to determine if the passed
    /// interface object has been retained, and if so, ownership is transferred
    /// to the caller.
    ///
    /// @param iface raw interface pointer (from C, usually)
    ///
    /// @returns Pointer owning the submitted interface object, or an empty
    /// pointer if NULL passed or not found.
    OSVR_CLIENT_EXPORT::osvr::client::ClientInterfacePtr
    releaseInterface(::osvr::client::ClientInterface *iface);

    InterfaceList const &getInterfaces() const { return m_interfaces; }

    /// @brief Gets a string parameter value.
    OSVR_CLIENT_EXPORT std::string
    getStringParameter(std::string const &path) const;

    /// @brief Sends a JSON route/transform object to the server.
    OSVR_CLIENT_EXPORT void sendRoute(std::string const &route);

    /// @brief Sets a string parameter value.
    void setParameter(std::string const &path, std::string const &value);

  protected:
    /// @brief Constructor for derived class use only.
    OSVR_ClientContextObject(const char appId[]);

  private:
    virtual void m_update() = 0;
    virtual void m_sendRoute(std::string const &route);
    std::string const m_appId;
    InterfaceList m_interfaces;
    std::map<std::string, std::string> m_params;
};

#endif // INCLUDED_ContextImpl_h_GUID_9000C62E_3693_4888_83A2_0D26F4591B6A
