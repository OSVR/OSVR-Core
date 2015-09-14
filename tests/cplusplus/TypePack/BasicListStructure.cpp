/** @file
    @brief Test Implementation

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>

*/

// Copyright 2015 Sensics, Inc.
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

// Internal Includes
#include "TypePackTestShared.h"

// Yes, I know these are all static (compile-time) tests, but using the gtest
// structure to split them into logical units.
TEST(TypePack, basicListStructure) {
    static_assert(is_same<tp::t_<mylist>, tp::t_<tp::list<mylist>>>::value,
                  "Unwrapping list of single list");
    static_assert(
        is_same<tp::t_<mylist>, tp::t_<tp::list<tp::list<mylist>>>>::value,
        "Three-level unwrapping list of single list");
    static_assert(tp::size<mylist>::value == mylist_len, "correct length");
    static_assert(is_same<mylist, tp::t_<mylist>>::value,
                  "Result of list is list");
    static_assert(is_same<mylist, tp::coerce_list<mylist>>::value,
                  "Coerce list from list");
    static_assert(
        is_same<mylist, tp::coerce_list<tp::list<tp::list<mylist>>>>::value,
        "Coerce list from double-wrapped list");
}
