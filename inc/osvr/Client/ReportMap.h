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

#ifndef INCLUDED_ReportMap_h_GUID_E6AD6837_0C7A_4FAF_0639_4F11821AEE65
#define INCLUDED_ReportMap_h_GUID_E6AD6837_0C7A_4FAF_0639_4F11821AEE65

// Internal Includes
#include <osvr/Client/ReportTypes.h>

// Library/third-party includes
#include <boost/fusion/include/mpl.hpp>
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/pair.hpp>
#include <boost/mpl/transform.hpp>

// Standard includes
// - none

namespace osvr {
namespace client {
    namespace traits {

        namespace detail {
            struct FusionMakePair {
                template <typename T1, typename T2> struct apply {
                    typedef boost::fusion::pair<T1, T2> type;
                };
            };
        } // namespace detail

        /// @brief Given an operation to transform a report type into the
        /// corresponding desired map value type, generates a Boost Fusion map
        /// from report types to values of the desired types.
        template <typename ValueTypeTransform> struct GenerateReportMap {
            typedef typename boost::mpl::transform<
                ReportTypes, ValueTypeTransform>::type ValueTypes;
            typedef typename boost::mpl::transform<ReportTypes, ValueTypes,
                                                   detail::FusionMakePair>::type
                PairSequence;
            typedef
                typename boost::fusion::result_of::as_map<PairSequence>::type
                    type;
        };
    } // namespace traits
} // namespace client
} // namespace osvr
#endif // INCLUDED_ReportMap_h_GUID_E6AD6837_0C7A_4FAF_0639_4F11821AEE65
