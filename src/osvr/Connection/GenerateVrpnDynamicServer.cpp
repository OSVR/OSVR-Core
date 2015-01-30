/** @file
    @brief Implementation

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

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

// Standard includes
// - none

namespace osvr {
namespace connection {

    namespace detail {
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
        typedef boost::mpl::begin<detail::ServerTypes>::type Begin;
        typedef boost::mpl::end<ServerTypes>::type End;

        template <typename Iter = Begin,
                  typename Result = boost::mpl::vector0<>,
                  bool = boost::is_same<Iter, End>::value>
        struct FilterAndGenerate {
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
                } else {
                    return NextWithout::run(init);
                }
            }
        };
        template <typename Result, bool IsEmpty> struct FinishGeneration;
        /// @brief Don't attempt to construct with an empty list.
        template <typename Result> struct FinishGeneration<Result, true> {
            static vrpn_MainloopObject *run(DeviceConstructionData &) {
                return nullptr;
            }
        };
        /// @brief Normal construction case.
        template <typename Result> struct FinishGeneration<Result, false> {
            static vrpn_MainloopObject *run(DeviceConstructionData &init) {
                return vrpn_MainloopObject::wrap(
                    GenerateServer<Result>::run(init));
            }
        };

        template <typename Iter, typename Result>
        struct FilterAndGenerate<Iter, Result, true> {
            static vrpn_MainloopObject *run(DeviceConstructionData &init) {
                typedef typename boost::mpl::empty<Result>::type IsEmpty;
                return FinishGeneration<Result, IsEmpty::value>::run(init);
            }
        };

    } // namespace detail

    vrpn_MainloopObject *
    generateVrpnDynamicServer(DeviceConstructionData &init) {
        return detail::FilterAndGenerate<>::run(init);
    }
} // namespace connection
} // namespace osvr