/** @file
    @brief Implementation

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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include "GenerateVrpnDynamicServer.h"
#include "VrpnBaseFlexServer.h"
#include "VrpnAnalogServer.h"
#include "VrpnButtonServer.h"
#include "VrpnTrackerServer.h"
#include "GenerateCompoundServer.h"

// Library/third-party includes
#include <boost/mpl/vector.hpp>
#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/next.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/utility.hpp>

// Standard includes
// - none

namespace osvr {
namespace connection {

    namespace server_generation {
        typedef boost::mpl::vector<vrpn_BaseFlexServer, VrpnAnalogServer,
                                   VrpnButtonServer,
                                   VrpnTrackerServer> ServerTypes;
        template <typename T> struct ShouldInclude {
            static bool predicate(DeviceConstructionData &);
        };

        template <>
        bool ShouldInclude<vrpn_BaseFlexServer>::predicate(
            DeviceConstructionData &) {
            return true;
        }

        template <>
        bool ShouldInclude<VrpnAnalogServer>::predicate(
            DeviceConstructionData &init) {
            return init.obj.getAnalogs().is_initialized();
        }

        template <>
        bool ShouldInclude<VrpnButtonServer>::predicate(
            DeviceConstructionData &init) {
            return init.obj.getButtons().is_initialized();
        }

        template <>
        bool ShouldInclude<VrpnTrackerServer>::predicate(
            DeviceConstructionData &init) {
            return init.obj.getTracker();
        }
        typedef boost::mpl::begin<ServerTypes>::type Begin;
        typedef boost::mpl::end<ServerTypes>::type End;

        /// @brief Template to invoke construction once sequence filtering is
        /// complete.
        template <typename Result, typename Enable = void>
        struct MakeServerFromSequence;

        /// @brief Don't attempt to construct with an empty sequence.
        template <typename Result>
        struct MakeServerFromSequence<
            Result, typename boost::enable_if<
                        typename boost::mpl::empty<Result>::type>::type> {
            static vrpn_MainloopObject *make(DeviceConstructionData &) {
                return nullptr;
            }
        };
        /// @brief Normal construction case.
        template <typename Result>
        struct MakeServerFromSequence<
            Result, typename boost::disable_if<
                        typename boost::mpl::empty<Result>::type>::type> {
            static vrpn_MainloopObject *make(DeviceConstructionData &init) {
                return vrpn_MainloopObject::wrap(
                    GenerateServer<Result>::make(init));
            }
        };

        /// Template to perform the list filtering.
        template <typename Iter = Begin,
                  typename Result = boost::mpl::vector0<>,
                  typename Enable = void>
        struct FilterAndGenerate;

        /// Recursive case: Iter is not past the end.
        template <typename Iter, typename Result>
        struct FilterAndGenerate<
            Iter, Result,
            typename boost::disable_if<boost::is_same<Iter, End> >::type> {
            static vrpn_MainloopObject *run(DeviceConstructionData &init) {
                typedef typename boost::mpl::deref<Iter>::type CurrentType;
                typedef typename boost::mpl::next<Iter>::type NextIter;

                typedef
                    typename boost::mpl::push_back<Result, CurrentType>::type
                        ExtendedResult;
                typedef FilterAndGenerate<NextIter, ExtendedResult> NextWith;
                typedef FilterAndGenerate<NextIter, Result> NextWithout;
                if (ShouldInclude<CurrentType>::predicate(init)) {
                    return NextWith::run(init);
                }
                return NextWithout::run(init);
            }
        };

        /// Base case: iter is past the end.
        template <typename Iter, typename Result>
        struct FilterAndGenerate<
            Iter, Result,
            typename boost::enable_if<boost::is_same<Iter, End> >::type> {
            static vrpn_MainloopObject *run(DeviceConstructionData &init) {
                return MakeServerFromSequence<Result>::make(init);
            }
        };

    } // namespace server_generation

    vrpn_MainloopObject *
    generateVrpnDynamicServer(server_generation::ConstructorArgument &init) {
        return server_generation::FilterAndGenerate<>::run(init);
    }
} // namespace connection
} // namespace osvr
