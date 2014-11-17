/** @file
    @brief Header providing a templated deleter function

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

#ifndef INCLUDED_GenericDeleter_h_GUID_FF10B285_AE30_45B6_BFD4_D84F98FB1EF0
#define INCLUDED_GenericDeleter_h_GUID_FF10B285_AE30_45B6_BFD4_D84F98FB1EF0

namespace osvr {
namespace util {
    /// @brief Generic deleter function, wrapping the stock delete call.
    ///
    /// Used in header-only C++ wrappers over C APIs.
    template <typename T> inline void generic_deleter(void *obj) {
        T *o = static_cast<T *>(obj);
        delete o;
    }
} // namespace util
} // namespace osvr

#endif // INCLUDED_GenericDeleter_h_GUID_FF10B285_AE30_45B6_BFD4_D84F98FB1EF0
