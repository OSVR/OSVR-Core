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
    return (boost::get<ElementType const>(&elt) != nullptr);
}

template <typename ElementType>
inline bool isNodeType(osvr::routing::PathNode const &node) {
    return isElementType<ElementType>(node.value());
}

#endif // INCLUDED_IsType_h_GUID_F2A39A30_F0D0_4288_0E1B_C5B982CD3BED
