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

#ifndef INCLUDED_ReportMap_h_GUID_E6AD6837_0C7A_4FAF_0639_4F11821AEE65
#define INCLUDED_ReportMap_h_GUID_E6AD6837_0C7A_4FAF_0639_4F11821AEE65

// Internal Includes
#include <osvr/Common/ReportTypes.h>

// Library/third-party includes
#include <boost/fusion/include/mpl.hpp>
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/pair.hpp>
#include <boost/mpl/transform.hpp>

// Standard includes
// - none

namespace osvr {
namespace common {
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
} // namespace common
} // namespace osvr
#endif // INCLUDED_ReportMap_h_GUID_E6AD6837_0C7A_4FAF_0639_4F11821AEE65
