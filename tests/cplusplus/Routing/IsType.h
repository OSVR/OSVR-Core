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

#ifndef INCLUDED_IsType_h_GUID_F2A39A30_F0D0_4288_0E1B_C5B982CD3BED
#define INCLUDED_IsType_h_GUID_F2A39A30_F0D0_4288_0E1B_C5B982CD3BED

// Internal Includes
#include <osvr/Routing/PathNode.h>

// Library/third-party includes
#include <boost/variant/get.hpp>

// Standard includes
// - none

template <typename ElementType>
inline bool isElementType(osvr::routing::elements::PathElement const &elt) {
    return (boost::get<ElementType const>(&elt) != NULL);
}

template <typename ElementType>
inline bool isNodeType(osvr::routing::PathNode const &node) {
    return isElementType<ElementType>(node.value());
}

#endif // INCLUDED_IsType_h_GUID_F2A39A30_F0D0_4288_0E1B_C5B982CD3BED
