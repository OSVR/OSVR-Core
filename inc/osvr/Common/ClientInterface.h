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

#ifndef INCLUDED_ClientInterface_h_GUID_A3A55368_DE2F_4980_BAE9_1C398B0D40A1
#define INCLUDED_ClientInterface_h_GUID_A3A55368_DE2F_4980_BAE9_1C398B0D40A1

// Internal Includes
#include <osvr/Common/Export.h>
#include <osvr/Common/ClientContext_fwd.h>
#include <osvr/Common/ClientInterfacePtr.h>
#include <osvr/Common/InterfaceState.h>
#include <osvr/Common/InterfaceCallbacks.h>
#include <osvr/Common/StateType.h>
#include <osvr/Common/ReportStateTraits.h>
#include <osvr/Common/Tracing.h>
#include <osvr/Util/ClientOpaqueTypesC.h>
#include <osvr/Util/ClientCallbackTypesC.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>
#include <boost/any.hpp>

// Standard includes
#include <string>
#include <vector>
#include <functional>

struct OSVR_ClientInterfaceObject : boost::noncopyable {

  protected:
    /// @brief Constructor - only to be called by a factory function.
    OSVR_COMMON_EXPORT
    OSVR_ClientInterfaceObject(osvr::common::ClientContext &ctx,
                               std::string const &path);

  public:
    ~OSVR_ClientInterfaceObject() {
        osvr::common::tracing::markReleaseInterface(m_path);
    }

    /// @brief Get the path as a string.
    OSVR_COMMON_EXPORT std::string const &getPath() const;

    /// @name State-related wrapper methods
    /// @brief Primarily forwarding to the nested InterfaceState instance.
    /// @{
    /// @brief If state exists for the given ReportType on this interface, it
    /// will be returned in the arguments, and true will be returned.
    template <typename ReportType>
    bool
    getState(osvr::util::time::TimeValue &timestamp,
             osvr::common::traits::StateFromReport_t<ReportType> &state) const {
        osvr::common::tracing::markGetState(m_path);
        if (!m_state.hasState<ReportType>()) {
            return false;
        }
        m_state.getState<ReportType>(timestamp, state);
        return true;
    }

    template <typename ReportType> bool hasStateForReportType() const {
        return m_state.hasState<ReportType>();
    }

    bool hasAnyState() const { return m_state.hasAnyState(); }

    /// @brief Set saved state for a report type.
    template <typename ReportType>
    void setState(const OSVR_TimeValue &timestamp, ReportType const &report) {
        static_assert(
            osvr::common::traits::KeepStateForReport<ReportType>::value,
            "Should only call setState if we're keeping state for this report "
            "type!");
        m_state.setStateFromReport(timestamp, report);
    }
    /// @}

    /// @name Callback-related wrapper methods
    /// @brief Primarily forwarding to the nested InterfaceCallbacks instance.
    /// @{
    /// @brief Register a callback for a known report type.
    template <typename CallbackType>
    void registerCallback(CallbackType cb, void *userdata) {
        m_callbacks.addCallback(cb, userdata);
    }

    /// @brief Trigger all callbacks for the given known report
    /// type.
    template <typename ReportType>
    void triggerCallbacks(const OSVR_TimeValue &timestamp,
                          ReportType const &report) {
        m_callbacks.triggerCallbacks(timestamp, report);
    }

    /// @brief Get the number of registered callbacks for the given report type.
    template <typename ReportType>
    std::size_t getNumCallbacksFor(ReportType const &r) const {
        return m_callbacks.getNumCallbacksFor(r);
    }
    /// @}

    /// @brief Update any state.
    void update();

    osvr::common::ClientContext &getContext() const { return m_ctx; }

    /// @brief Access the type-erased data for this interface.
    boost::any &data() { return m_data; }

  private:
    osvr::common::ClientContext &m_ctx;
    std::string const m_path;
    osvr::common::InterfaceCallbacks m_callbacks;
    osvr::common::InterfaceState m_state;
    boost::any m_data;
};

#endif // INCLUDED_ClientInterface_h_GUID_A3A55368_DE2F_4980_BAE9_1C398B0D40A1
