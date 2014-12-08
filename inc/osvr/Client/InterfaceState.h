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

#ifndef INCLUDED_InterfaceState_h_GUID_FFF8951B_3677_4EB5_373A_3A1A697AECDE
#define INCLUDED_InterfaceState_h_GUID_FFF8951B_3677_4EB5_373A_3A1A697AECDE

// Internal Includes
#include <osvr/Client/ReportTypes.h>
#include <osvr/Util/TimeValue.h>

// Library/third-party includes
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/pair.hpp>
#include <boost/fusion/include/has_key.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/fusion/include/mpl.hpp>
#include <boost/optional.hpp>

// Standard includes
// - none

namespace osvr {
namespace client {
    /// @brief A templated type containing state and a timestamp for known,
    /// specialized report types.
    template <typename ReportType> struct StateMapContents {
        typedef typename traits::StateType<ReportType>::type state_type;
        state_type state;
        util::time::TimeValue timestamp;
    };

    /// @brief Metafunction taking a report type and returning a fusion pair to
    /// serve as an element of StateMap
    template <typename ReportType> struct StateMapType {
        typedef boost::fusion::pair<
            ReportType, boost::optional<StateMapContents<ReportType> > > type;
    };

    /// @brief Data structure mapping from a report type to an optional state
    /// value.
    typedef boost::fusion::result_of::as_map<boost::mpl::transform<
        traits::ReportTypes, StateMapType<boost::mpl::_> >::type>::type
        StateMap;

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
            boost::fusion::at_key<ReportType>(m_states) = c;
        }

        template <typename ReportType> bool hasState() const {
            return boost::fusion::at_key<ReportType>(m_states);
        }

        template <typename ReportType>
        void
        getState(util::time::TimeValue &timestamp,
                 typename traits::StateType<ReportType>::type &state) const {
            if (hasState<ReportType>()) {
                timestamp = boost::fusion::at_key<ReportType>(m_states)->timestamp;
                state = boost::fusion::at_key<ReportType>(m_states)->state;
            }
            /// @todo do we fail silently or throw exception if we are asked for
            /// state we don't have?
        }

      private:
        StateMap m_states;
    };

} // namespace client
} // namespace osvr

#endif // INCLUDED_InterfaceState_h_GUID_FFF8951B_3677_4EB5_373A_3A1A697AECDE
