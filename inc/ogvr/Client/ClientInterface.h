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

#ifndef INCLUDED_ClientInterface_h_GUID_A3A55368_DE2F_4980_BAE9_1C398B0D40A1
#define INCLUDED_ClientInterface_h_GUID_A3A55368_DE2F_4980_BAE9_1C398B0D40A1

// Internal Includes
#include <ogvr/Client/Export.h>
#include <ogvr/Client/ClientContext_fwd.h>
#include <ogvr/Client/ClientInterfacePtr.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>

struct OGVR_ClientInterfaceObject : boost::noncopyable {
  private:
    struct PrivateConstructor {};

  public:
    /// @brief Constructor - only to be called by ClientContext
    OGVR_ClientInterfaceObject(::ogvr::client::ClientContext *ctx,
                               std::string const &path,
                               PrivateConstructor const &);

    /// @brief Get the path as a string.
    std::string const &getPath() const;

    /// @brief Get the owning context.
    OGVR_CLIENT_EXPORT::ogvr::client::ClientContext &getContext();

    /// @brief Update any state.
    void update();

  private:
    ::ogvr::client::ClientContext *m_ctx;
    std::string const m_path;
    friend struct OGVR_ClientContextObject;
};

#endif // INCLUDED_ClientInterface_h_GUID_A3A55368_DE2F_4980_BAE9_1C398B0D40A1
