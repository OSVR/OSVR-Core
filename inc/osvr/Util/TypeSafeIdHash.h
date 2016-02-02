/** @file
    @brief Header defining specializations of std::hash that forward to the
   contained integer type, primarily to allow TypeSafeId values to be used in as
   keys in unordered_map<> and friends.

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

#ifndef INCLUDED_TypeSafeIdHash_h_GUID_1F2936E7_99CB_4D82_9B8D_1D994E2E17F0
#define INCLUDED_TypeSafeIdHash_h_GUID_1F2936E7_99CB_4D82_9B8D_1D994E2E17F0

// Internal Includes
#include <osvr/Util/TypeSafeId.h>

// Library/third-party includes
// - none

// Standard includes
#include <functional>

namespace std {
template <typename Tag> struct hash<osvr::util::TypeSafeId<Tag>> {
    using TypeSafeIdKeyType = osvr::util::TypeSafeId<Tag>;
    using WrappedType = typename TypeSafeIdKeyType::wrapped_type;
    size_t operator()(const TypeSafeIdKeyType &x) const {
        return std::hash<WrappedType>{}(x.value());
    }
};
} // namespace std

#endif // INCLUDED_TypeSafeIdHash_h_GUID_1F2936E7_99CB_4D82_9B8D_1D994E2E17F0
