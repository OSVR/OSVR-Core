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

#ifndef INCLUDED_RemoteHandlerInternals_h_GUID_20139712_D173_4D91_A995_2552DF68354A
#define INCLUDED_RemoteHandlerInternals_h_GUID_20139712_D173_4D91_A995_2552DF68354A

// Internal Includes
#include <osvr/Common/InterfaceList.h>
#include <osvr/Common/ClientInterface.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace client {
    /// @brief Class holding shared implementation between the various handlers.
    /// Primarily used to avoid the need for loops and move the point of
    /// transfer for the reports out of the individual handlers to have fewer
    /// places to change if (when) that implementation detail is altered.
    class RemoteHandlerInternals {
      public:
        /// Construct with a reference to an interface list.
        explicit RemoteHandlerInternals(common::InterfaceList &ifaces)
            : m_interfaces(ifaces) {}

        // non-assignable
        RemoteHandlerInternals &operator=(RemoteHandlerInternals &) = delete;

        /// @brief Set state and call callbacks for a report type.
        template <typename ReportType>
        void setStateAndTriggerCallbacks(const OSVR_TimeValue &timestamp,
                                         ReportType const &report) {
            static_assert(
                osvr::common::traits::KeepStateForReport<ReportType>::value,
                "Should only call a state setter if we're keeping state for "
                "this report type!");

            forEachInterface(
                [&timestamp, &report](common::ClientInterface &iface) {
                    iface.setState(timestamp, report);
                    iface.triggerCallbacks(timestamp, report);
                });
        }

        /// @brief Do something with every client interface object, if the above
        /// options don't suit your needs.
        template <typename F> void forEachInterface(F &&f) {
            common::ClientInterfacePtr pin;
            for (auto &iface : m_interfaces) {
                /// @todo is this needed? it increments a shared_ptr reference
                /// count which is expensive.
                pin = iface;
                f(*pin);
            }
        }

      private:
        common::InterfaceList &m_interfaces;
    };
} // namespace client
} // namespace osvr
#endif // INCLUDED_RemoteHandlerInternals_h_GUID_20139712_D173_4D91_A995_2552DF68354A
