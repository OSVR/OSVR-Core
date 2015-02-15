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

#ifndef INCLUDED_ClientInterface_h_GUID_A3A55368_DE2F_4980_BAE9_1C398B0D40A1
#define INCLUDED_ClientInterface_h_GUID_A3A55368_DE2F_4980_BAE9_1C398B0D40A1

// Internal Includes
#include <osvr/Client/Export.h>
#include <osvr/Client/ClientContext_fwd.h>
#include <osvr/Client/ClientInterfacePtr.h>
#include <osvr/Client/InterfaceState.h>
#include <osvr/Client/InterfaceCallbacks.h>
#include <osvr/Client/StateType.h>
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

    /// @brief If state exists for the given ReportType on this interface, it
    /// will be returned in the arguments, and true will be returned.
    template <typename ReportType>
    bool getState(osvr::util::time::TimeValue &timestamp,
                  typename osvr::client::traits::StateType<ReportType>::type &
                      state) const {
        if (!m_state.hasState<ReportType>()) {
            return false;
        }
        m_state.getState<ReportType>(timestamp, state);
        return true;
    }

    /// @brief Register a callback for a known report type.
    template <typename CallbackType>
    void registerCallback(CallbackType cb, void *userdata) {
        m_callbacks.addCallback(cb, userdata);
    }

    /// @brief Save state and trigger all callbacks for the given known report
    /// type.
    template <typename ReportType>
    void triggerCallbacks(const OSVR_TimeValue &timestamp,
                          ReportType const &report) {
        m_state.setStateFromReport(timestamp, report);
        m_callbacks.triggerCallbacks(timestamp, report);
    }

    /// @brief Update any state.
    void update();

  private:
    ::osvr::client::ClientContext *m_ctx;
    std::string const m_path;
    osvr::client::InterfaceCallbacks m_callbacks;
    osvr::client::InterfaceState m_state;
    friend struct OSVR_ClientContextObject;
};

#endif // INCLUDED_ClientInterface_h_GUID_A3A55368_DE2F_4980_BAE9_1C398B0D40A1
