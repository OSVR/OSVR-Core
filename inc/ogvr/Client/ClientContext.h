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
#include <ogvr/Client/Export.h>
#include <ogvr/Client/ClientContext_fwd.h>
#include <ogvr/Client/ClientInterfacePtr.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>
#include <vector>

struct OGVR_ClientContextObject : boost::noncopyable {
  public:
    typedef std::vector<::ogvr::client::ClientInterfacePtr> InterfaceList;
    /// @brief Destructor
    virtual ~OGVR_ClientContextObject();

    /// @brief System-wide update method.
    OGVR_CLIENT_EXPORT void update();

    /// @brief Accessor for app ID
    std::string const &getAppId() const;

    /// @brief Creates an interface object for the given path. The context
    /// retains shared ownership.
    ///
    /// @param path Path to a resource. Should be absolute.
    OGVR_CLIENT_EXPORT::ogvr::client::ClientInterfacePtr
    getInterface(const char path[]);

    /// @brief Searches through this context to determine if the passed
    /// interface object has been retained, and if so, ownership is transferred
    /// to the caller.
    ///
    /// @param iface raw interface pointer (from C, usually)
    ///
    /// @returns Pointer owning the submitted interface object, or an empty
    /// pointer if NULL passed or not found.
    OGVR_CLIENT_EXPORT::ogvr::client::ClientInterfacePtr
    releaseInterface(::ogvr::client::ClientInterface *iface);

    InterfaceList const &getInterfaces() const { return m_interfaces; }

  protected:
    /// @brief Constructor for derived class use only.
    OGVR_ClientContextObject(const char appId[]);

  private:
    virtual void m_update() = 0;
    std::string const m_appId;
    InterfaceList m_interfaces;
};

#endif // INCLUDED_ContextImpl_h_GUID_9000C62E_3693_4888_83A2_0D26F4591B6A
