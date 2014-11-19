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

#ifndef INCLUDED_TreeNode_fwd_h_GUID_7AEB7FE7_1E26_478B_61DD_D39292047B03
#define INCLUDED_TreeNode_fwd_h_GUID_7AEB7FE7_1E26_478B_61DD_D39292047B03

// Internal Includes
#include <osvr/Util/SharedPtr.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace client {
    template <typename ValueType>
    class TreeNode;
    
    /// @brief Dummy struct containing the ownership pointer type for a
    /// TreeNode. (Actually a metafunction...)
    template <typename ValueType>
    struct TreeNodePointer {
        typedef shared_ptr<TreeNode<ValueType> > type;
    };
} // namespace client
} // namespace osvr

#endif // INCLUDED_TreeNode_fwd_h_GUID_7AEB7FE7_1E26_478B_61DD_D39292047B03
