/** @file
    @brief Header providing a template function to reset a range of smart
   pointers.

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

#ifndef INCLUDED_ResetPointerList_h_GUID_067DF9C9_67F7_4293_94EF_DBC5B5266801
#define INCLUDED_ResetPointerList_h_GUID_067DF9C9_67F7_4293_94EF_DBC5B5266801

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace util {

    /// @brief Reset every smart pointer in a container one by one
    template <typename RangeType>
    inline void resetPointerRange(RangeType range) {
        typedef typename RangeType::type IteratorType;
        typedef typename IteratorType::value_type PointerType;
        for (auto &ptr : range) {
            ptr.reset()
        }
    }
} // namespace util
} // namespace osvr

#endif // INCLUDED_ResetPointerList_h_GUID_067DF9C9_67F7_4293_94EF_DBC5B5266801
