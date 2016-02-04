/** @file
    @brief Header

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
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

#ifndef INCLUDED_BeaconIdTypes_h_GUID_8C85DE41_5CAC_4DA2_6C2A_962680202E1B
#define INCLUDED_BeaconIdTypes_h_GUID_8C85DE41_5CAC_4DA2_6C2A_962680202E1B

// Internal Includes
#include "BodyIdTypes.h"

// Library/third-party includes
#include <osvr/Util/TypeSafeId.h>
#include <boost/assert.hpp>

// Standard includes
#include <stdexcept>

namespace osvr {
namespace vbtracker {
    namespace detail {
        /// Type tag for type-safe zero-based beacon id
        struct ZeroBasedBeaconIdTag;
        /// Type tag for type-safe one-based beacon id
        struct OneBasedBeaconIdTag;

    } // namespace detail

    /// All beacon IDs, whether 0 or 1 based, are ints on the inside.
    using UnderlyingBeaconIdType = int;
} // namespace vbtracker
} // namespace osvr

namespace osvr {
namespace util {
    namespace typesafeid_traits {
        /// Tag-based specialization of underlying value type for beacon ID
        template <>
        struct WrappedType<vbtracker::detail::ZeroBasedBeaconIdTag> {
            using type = vbtracker::UnderlyingBeaconIdType;
        };
        /// Tag-based specialization of underlying value type for beacon ID
        template <> struct WrappedType<vbtracker::detail::OneBasedBeaconIdTag> {
            using type = vbtracker::UnderlyingBeaconIdType;
        };
    } // namespace typesafeid_traits

} // namespace util
} // namespace osvr

namespace osvr {
namespace vbtracker {
    /// Type-safe zero-based beacon ID.
    using ZeroBasedBeaconId = util::TypeSafeId<detail::ZeroBasedBeaconIdTag>;
    /// Type-safe one-based beacon ID.
    using OneBasedBeaconId = util::TypeSafeId<detail::OneBasedBeaconIdTag>;

    /// Overloaded conversion function to turn any beacon ID into one-based,
    /// respecting the convention that negative values don't change.
    inline OneBasedBeaconId makeOneBased(ZeroBasedBeaconId id) {
        OneBasedBeaconId ret;
        if (id.empty()) {
            return ret;
        } else if (id.value() < 0) {
            ret = OneBasedBeaconId(id.value());
        } else {
            ret = OneBasedBeaconId(id.value() + 1);
        }
        return ret;
    }

    /// No-op overload, so you can take any beacon ID and ensure it is
    /// one-based.
    inline OneBasedBeaconId const &makeOneBased(OneBasedBeaconId const &id) {
        return id;
    }

    /// Overloaded conversion function to turn any beacon ID into zero-based,
    /// respecting the convention that negative values don't change.
    inline ZeroBasedBeaconId makeZeroBased(OneBasedBeaconId id) {
        ZeroBasedBeaconId ret;
        if (id.empty()) {
            return ret;
        } else if (id.value() < 0) {
            ret = ZeroBasedBeaconId(id.value());
        } else {
            ret = ZeroBasedBeaconId(id.value() - 1);
        }
        return ret;
    }

    /// No-op overload, so you can take any beacon ID and ensure it is
    /// zero-based.
    inline ZeroBasedBeaconId const &makeZeroBased(ZeroBasedBeaconId const &id) {
        return id;
    }

    /// Does the given beacon ID indicate that it's identified?
    inline bool beaconIdentified(ZeroBasedBeaconId id) {
        return (!id.empty() && id.value() >= 0);
    }
    /// Does the given beacon ID indicate that it's identified?
    inline bool beaconIdentified(OneBasedBeaconId id) {
        return (!id.empty() && id.value() > 0);
    }

    /// Turn a (valid non-sentinel, i.e. identified) beacon id into an array
    /// index.
    inline std::size_t asIndex(ZeroBasedBeaconId id) {
        BOOST_ASSERT_MSG(beaconIdentified(id), "A beacon id must correspond to "
                                               "an identified beacon to be "
                                               "used as an index!");
        if (!beaconIdentified(id)) {
            throw std::logic_error("A beacon id must correspond to an "
                                   "identified beacon to be used as an index!");
        }
        return id.value();
    }

    /// @overload
    inline std::size_t asIndex(OneBasedBeaconId id) {
        return asIndex(makeZeroBased(id));
    }

} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_BeaconIdTypes_h_GUID_8C85DE41_5CAC_4DA2_6C2A_962680202E1B
