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
#include <osvr/Client/Export.h>
#include <osvr/Client/ClientContext_fwd.h>
#include <osvr/Client/ClientInterfacePtr.h>
#include <osvr/Util/ClientOpaqueTypesC.h>
#include <osvr/Util/ClientCallbackTypesC.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>
#include <vector>
#include <functional>

struct OSVR_ClientInterfaceObject : boost::noncopyable {
  private:
    struct PrivateConstructor {};

  public:
    /// @brief Constructor - only to be called by ClientContext
    OSVR_ClientInterfaceObject(::osvr::client::ClientContext *ctx,
                               std::string const &path,
                               PrivateConstructor const &);

    /// @brief Get the path as a string.
    std::string const &getPath() const;

    /// @brief Get the owning context.
    OSVR_CLIENT_EXPORT::osvr::client::ClientContext &getContext();

    OSVR_CLIENT_EXPORT void registerCallback(OSVR_PoseCallback cb,
                                             void *userdata);

    void triggerCallbacks(const OSVR_TimeValue &timestamp,
                          const OSVR_PoseReport &report);

    /// @brief Update any state.
    void update();

  private:
    std::vector<std::function<void(const OSVR_TimeValue *,
                                   const OSVR_PoseReport *)> > m_trackerCB;

    ::osvr::client::ClientContext *m_ctx;
    std::string const m_path;
    friend struct OSVR_ClientContextObject;
};

#endif // INCLUDED_ClientInterface_h_GUID_A3A55368_DE2F_4980_BAE9_1C398B0D40A1
