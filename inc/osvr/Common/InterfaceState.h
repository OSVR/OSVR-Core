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

#ifndef INCLUDED_InterfaceState_h_GUID_FFF8951B_3677_4EB5_373A_3A1A697AECDE
#define INCLUDED_InterfaceState_h_GUID_FFF8951B_3677_4EB5_373A_3A1A697AECDE

// Internal Includes
#include <osvr/Common/ReportTypes.h>
#include <osvr/Common/StateType.h>
#include <osvr/Common/ReportState.h>
#include <osvr/Util/TimeValue.h>
#include <osvr/Common/Tracing.h>
#include <osvr/TypePack/TypeKeyedTuple.h>
#include <osvr/TypePack/Quote.h>

// Library/third-party includes
#include <boost/optional.hpp>

// Standard includes
// - none

namespace osvr {
namespace common {
    /// @brief A templated type containing state and a timestamp for known,
    /// specialized report types.
    template <typename ReportType> struct StateMapContents {
        using state_type = traits::StateFromReport_t<ReportType>;
        state_type state;
        util::time::TimeValue timestamp;
    };

    /// @brief Alias taking a report type and returning a state map
    /// value type.
    template <typename ReportType>
    using StateMapValueType = boost::optional<StateMapContents<ReportType>>;

    /// @brief Data structure mapping from a report type to an optional state
    /// value.
    using StateMap =
        typepack::TypeKeyedTuple<traits::ReportTypeList,
                                 typepack::quote<StateMapValueType>>;

    /// @brief Class to maintain state for an interface for each report (and
    /// thus state) type explicitly enumerated.
    class InterfaceState {
      public:
        template <typename ReportType>
        void setStateFromReport(util::time::TimeValue const &timestamp,
                                ReportType const &report) {
            StateMapContents<ReportType> c;
            c.state = reportState(report);
            c.timestamp = timestamp;
            typepack::get<ReportType, StateMap>(m_states) = c;
            m_hasState = true;
        }

        template <typename ReportType> bool hasState() const {
            // using typepack::get;
            return m_hasState && bool(typepack::cget<ReportType>(m_states));
        }

        bool hasAnyState() const { return m_hasState; }

        template <typename ReportType>
        void getState(util::time::TimeValue &timestamp,
                      traits::StateFromReport_t<ReportType> &state) const {
            if (hasState<ReportType>()) {
                timestamp = typepack::cget<ReportType>(m_states)->timestamp;
                state = typepack::cget<ReportType>(m_states)->state;
            }
            /// @todo do we fail silently or throw exception if we are asked for
            /// state we don't have?
        }

      private:
        StateMap m_states;
        bool m_hasState = false;
    };

} // namespace common
} // namespace osvr

#endif // INCLUDED_InterfaceState_h_GUID_FFF8951B_3677_4EB5_373A_3A1A697AECDE
