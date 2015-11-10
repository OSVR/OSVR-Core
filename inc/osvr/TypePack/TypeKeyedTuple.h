/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
// TypePack is part of OSVR-Core.
//
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef INCLUDED_TypeKeyedTuple_h_GUID_04E79266_BD2F_458D_B7AC_DF5F35CC6EC4
#define INCLUDED_TypeKeyedTuple_h_GUID_04E79266_BD2F_458D_B7AC_DF5F35CC6EC4

// Internal Includes
#include "TypeKeyed.h"
#include "Apply.h"
#include "Transform.h"
#include "ApplyList.h"

// Library/third-party includes
// - none

// Standard includes
#include <tuple>

namespace osvr {
namespace typepack {
    /// @brief Provides a data structure where a value of heterogeneous data
    /// types may be stored at runtime for each of the "key" types in a
    /// list. The runtime data type stored is computed by an alias class.
    ///
    /// Vaguely replaces the functionality of a boost::fusion::map.
    ///
    /// Values can be accessed with the nonmember get() free common to
    /// type-keyed containers.
    ///
    /// Element access performance is equal to `get()` on a std::tuple
    /// (should be constant)
    template <typename KeyList, typename ComputeValueTypes>
    class TypeKeyedTuple
        : public TypeKeyedBase<TypeKeyedTuple<KeyList, ComputeValueTypes>> {
        using value_types = transform<KeyList, ComputeValueTypes>;

      public:
        using key_types = KeyList;

        using container_type = apply_list<quote<std::tuple>, value_types>;

      private:
        template <typename, typename>
        friend struct typekeyed_detail::ValueAccessor;

        /// Internal method/implementation detail, do not use in consuming code!
        container_type &nested_container() { return container_; }
        /// Internal method/implementation detail, do not use in consuming code!
        container_type const &nested_container() const { return container_; }

      private:
        container_type container_;
    };

    // Required traits
    namespace typekeyed_detail {
        template <typename KeyList, typename ComputeValueTypes, typename Key>
        struct ValueTypeAtKeyTraits<TypeKeyedTuple<KeyList, ComputeValueTypes>,
                                    Key> {
            using type = apply<ComputeValueTypes, Key>;
        };
    } // namespace typekeyed_detail

} // namespace typepack
} // namespace osvr

#endif // INCLUDED_TypeKeyedTuple_h_GUID_04E79266_BD2F_458D_B7AC_DF5F35CC6EC4
