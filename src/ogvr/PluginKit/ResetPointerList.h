/** @file
    @brief Header

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>;
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_ResetPointerList_h_GUID_067DF9C9_67F7_4293_94EF_DBC5B5266801
#define INCLUDED_ResetPointerList_h_GUID_067DF9C9_67F7_4293_94EF_DBC5B5266801

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <algorithm>

namespace ogvr {
namespace detail {

    /// @brief Functor for resetting smart pointers, for use with for_each or
    /// similar.
    template <typename SmartPointerType> struct PointerResetter {
        void operator()(SmartPointerType &p) { p.reset(); }
    };

    /// @brief Reset every smart pointer in a container one by one, from
    /// beginning to end.
    template <typename ListType>
    inline void resetPointerList(ListType &ptrlist) {
        typedef typename ListType::value_type PointerType;
        std::for_each(ptrlist.begin(), ptrlist.end(),
                      PointerResetter<PointerType>());
    }
    /// @brief Reset every smart pointer in a container one by one in reverse
    /// order.
    template <typename ListType>
    inline void resetPointerListReverseOrder(ListType &ptrlist) {
        typedef typename ListType::value_type PointerType;
        std::for_each(ptrlist.rbegin(), ptrlist.rend(),
                      PointerResetter<PointerType>());
    }
}
}

#endif // INCLUDED_ResetPointerList_h_GUID_067DF9C9_67F7_4293_94EF_DBC5B5266801
