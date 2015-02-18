/** @file
    @brief Header

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

#ifndef INCLUDED_BufferTraits_h_GUID_83FE7C11_8192_409F_7D69_FDBC57B88581
#define INCLUDED_BufferTraits_h_GUID_83FE7C11_8192_409F_7D69_FDBC57B88581

// Internal Includes
#include <osvr/Common/Buffer_fwd.h>

// Library/third-party includes
#include <boost/type_traits/is_convertible.hpp>

// Standard includes
// - none

namespace osvr {
namespace common {
    namespace detail {
        /// @brief Serves to provide conversion operators for things that work
        /// like Buffers to assist with traits/static asserts.
        class BufferTypeChecker {
          public:
            template <typename ContainerType>
            BufferTypeChecker(Buffer<ContainerType> const &) {}
        };

        /// @brief Serves to provide conversion operators for things that work
        /// like BufferReaders to assist with traits/static asserts.
        class BufferReaderTypeChecker {
          public:
            template <typename ContainerType>
            BufferReaderTypeChecker(BufferReader<ContainerType> const &) {}
        };
    }

    /// @brief Type trait: is the given type a buffer?
    template <typename T>
    struct is_buffer : boost::is_convertible<T, detail::BufferTypeChecker> {};

    /// @brief Type trait: is the given type a buffer reader?
    template <typename T>
    struct is_buffer_reader
        : boost::is_convertible<T, detail::BufferReaderTypeChecker> {};

} // namespace common
} // namespace osvr
#endif // INCLUDED_BufferTraits_h_GUID_83FE7C11_8192_409F_7D69_FDBC57B88581
