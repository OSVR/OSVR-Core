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

TEST(TypePack, ifExpressions) {
    {
        using a = tp::if_<true_type>;
        using b = tp::if_c<true>;
    }
#if 0 // untestable?
    {
        using a = tp::if_<false_type>;
        using b = tp::if_c<false>;
    }
#endif
    static_assert(is_same<tp::if_<true_type, int>, int>::value, "if-then true");
    static_assert(is_same<tp::if_c<true, int>, int>::value, "if-then true");

#if 0 // untestable?
    static_assert(!is_same<tp::if_<false_type, int>, int>::value, "if-then false");
    static_assert(!is_same<tp::if_c<false, int>, int>::value, "if-then false");
#endif

    static_assert(is_same<tp::if_<true_type, int, float>, int>::value,
                  "if-then-else true");
    static_assert(is_same<tp::if_c<true, int, float>, int>::value,
                  "if-then-else true");
    static_assert(is_same<tp::if_<false_type, int, float>, float>::value,
                  "if-then-else false");
    static_assert(is_same<tp::if_c<false, int, float>, float>::value,
                  "if-then-else false");
}
