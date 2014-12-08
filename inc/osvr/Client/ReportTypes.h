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

#ifndef INCLUDED_ReportTypes_h_GUID_BB8F47AD_5F82_439E_C3F9_7C57D016D3C9
#define INCLUDED_ReportTypes_h_GUID_BB8F47AD_5F82_439E_C3F9_7C57D016D3C9

// Internal Includes
#include <osvr/Util/ClientCallbackTypesC.h>

// Library/third-party includes
#include <boost/mpl/vector.hpp>

// Standard includes
// - none

namespace osvr {
namespace client {
    /// @brief Type traits and metaprogramming supports related to
    /// specially-handled reports.
    namespace traits {
        /// @brief Metafunction taking a ReportType and returning the associated
        /// CallbackType
        template <typename T> struct CallbackType {};

        /// @brief Metafunction taking a ReportType and returning the associated
        /// StateType
        template <typename T> struct StateType {};

#define OSVR_REPORT_TRAITS(TYPE)                                               \
    template <> struct CallbackType<OSVR_##TYPE##Report> {                     \
        typedef OSVR_##TYPE##Callback type;                                    \
    };                                                                         \
    template <> struct StateType<OSVR_##TYPE##Report> {                        \
        typedef OSVR_##TYPE##State type;                                       \
    };
        OSVR_REPORT_TRAITS(Analog)
        OSVR_REPORT_TRAITS(Button)
        OSVR_REPORT_TRAITS(Pose)
        OSVR_REPORT_TRAITS(Position)
        OSVR_REPORT_TRAITS(Orientation)
#undef OSVR_REPORT_TRAITS

        /// @brief A typelist containing all specially-handled report types.
        typedef boost::mpl::vector<OSVR_AnalogReport, OSVR_ButtonReport,
                                   OSVR_PoseReport, OSVR_PositionReport,
                                   OSVR_OrientationReport> ReportTypes;

        /// @brief Helper traits struct for reportState()
        template <typename ReportType> struct ReportStateGetter {
            static typename StateType<ReportType>::type const &
            apply(ReportType const &r) {
                return r.state;
            }
            static typename StateType<ReportType>::type apply(ReportType &r) {
                return r.state;
            }
        };

        template <> struct ReportStateGetter<OSVR_PositionReport> {
            static OSVR_PositionState const &
            apply(OSVR_PositionReport const &r) {
                return r.xyz;
            }
            static OSVR_PositionState apply(OSVR_PositionReport &r) {
                return r.xyz;
            }
        };

        template <> struct ReportStateGetter<OSVR_PoseReport> {
            static OSVR_PoseState const &apply(OSVR_PoseReport const &r) {
                return r.pose;
            }
            static OSVR_PoseState apply(OSVR_PoseReport &r) { return r.pose; }
        };

        template <> struct ReportStateGetter<OSVR_OrientationReport> {
            static OSVR_OrientationState const &
            apply(OSVR_OrientationReport const &r) {
                return r.rotation;
            }
            static OSVR_OrientationState apply(OSVR_OrientationReport &r) {
                return r.rotation;
            }
        };
    } // namespace traits

    /// @brief Generic const accessor for the "state" member of a report.
    template <typename ReportType>
    typename traits::StateType<ReportType>::type const &
    reportState(ReportType const &r) {
        return traits::ReportStateGetter<ReportType>::apply(r);
    }

    /// @brief Generic accessor for the "state" member of a report.
    template <typename ReportType>
    typename traits::StateType<ReportType>::type &reportState(ReportType &r) {
        return traits::ReportStateGetter<ReportType>::apply(r);
    }
} // namespace client
} // namespace osvr

#endif // INCLUDED_ReportTypes_h_GUID_BB8F47AD_5F82_439E_C3F9_7C57D016D3C9
