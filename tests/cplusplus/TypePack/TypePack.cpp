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
#include <osvr/TypePack/TypePack.h>

// Library/third-party includes
#include "gtest/gtest.h"

// Standard includes
#include <type_traits>
#include <string>
#include <stdint.h>

using std::is_same;
using std::true_type;
using std::false_type;
namespace tp = osvr::typepack;

// THIS SECTION MUST BE MANUALLY KEPT IN-SYNC!
using myhead = uint32_t;
using myelt1 = myhead;
using myelt2 = int16_t;
using myelt3 = bool;
using mytail = tp::list<myelt2, myelt3>;
using myhead2 = tp::list<myelt1, myelt2>;
using mylist = tp::list<myelt1, myelt2, myelt3>;

static const size_t mylist_len = 3;
static_assert(is_same<mylist, tp::coerce_list<myelt1, myelt2, myelt3>>::value,
              "Coerce list from type pack");
// END MANUAL SYNC

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

TEST(TypePack, splitList) {
    static_assert(is_same<tp::head<mylist>, myhead>::value, "Correct head");
    static_assert(is_same<tp::tail<mylist>, mytail>::value, "Correct tail");
}
TEST(TypePack, push) {
    static_assert(is_same<tp::head<mylist>, myhead>::value, "Correct head");
    static_assert(is_same<mylist, tp::push_front<mytail, myhead>>::value,
                  "Push front");
    static_assert(is_same<mylist, tp::push_back<myhead2, myelt3>>::value,
                  "Push back");
}

TEST(TypePack, booleanAnd) {
    /// empty list - defined to be true
    static_assert(tp::and_<>::value == true, "Empty list");

    /// one argument
    static_assert(tp::and_<tp::bool_<true>>::value == true, "Single true");
    static_assert(tp::and_<tp::bool_<false>>::value == false, "Single false");

    /// two arguments
    static_assert(tp::and_<tp::bool_<true>, tp::bool_<true>>::value == true,
                  "Two true");
    static_assert(tp::and_<tp::bool_<false>, tp::bool_<false>>::value == false,
                  "Two false");

    static_assert(tp::and_<tp::bool_<false>, tp::bool_<true>>::value == false,
                  "1 true 1 false");
    static_assert(tp::and_<tp::bool_<true>, tp::bool_<false>>::value == false,
                  "1 true 1 false");

    /// three arguments
    static_assert(tp::and_<true_type, true_type, true_type>::value == true,
                  "Three true");
    static_assert(tp::and_<false_type, false_type, false_type>::value == false,
                  "Three false");

    static_assert(tp::and_<false_type, true_type, true_type>::value == false,
                  "2 true 1 false");
    static_assert(tp::and_<true_type, false_type, true_type>::value == false,
                  "2 true 1 false");
    static_assert(tp::and_<true_type, true_type, false_type>::value == false,
                  "2 true 1 false");

    static_assert(tp::and_<true_type, false_type, false_type>::value == false,
                  "1 true 2 false");
    static_assert(tp::and_<false_type, true_type, false_type>::value == false,
                  "1 true 2 false");
    static_assert(tp::and_<false_type, false_type, true_type>::value == false,
                  "1 true 2 false");
}

TEST(TypePack, booleanOr) {
    /// empty list - defined to be false
    static_assert(tp::or_<>::value == false, "Empty list");

    /// one argument
    static_assert(tp::or_<tp::bool_<true>>::value == true, "Single true");
    static_assert(tp::or_<tp::bool_<false>>::value == false, "Single false");

    /// two arguments
    static_assert(tp::or_<tp::bool_<true>, tp::bool_<true>>::value == true,
                  "Two true");
    static_assert(tp::or_<tp::bool_<false>, tp::bool_<false>>::value == false,
                  "Two false");

    static_assert(tp::or_<tp::bool_<false>, tp::bool_<true>>::value == true,
                  "1 true 1 false");
    static_assert(tp::or_<tp::bool_<true>, tp::bool_<false>>::value == true,
                  "1 true 1 false");

    /// three arguments
    static_assert(tp::or_<true_type, true_type, true_type>::value == true,
                  "Three true");
    static_assert(tp::or_<false_type, false_type, false_type>::value == false,
                  "Three false");

    static_assert(tp::or_<false_type, true_type, true_type>::value == true,
                  "2 true 1 false");
    static_assert(tp::or_<true_type, false_type, true_type>::value == true,
                  "2 true 1 false");
    static_assert(tp::or_<true_type, true_type, false_type>::value == true,
                  "2 true 1 false");

    static_assert(tp::or_<true_type, false_type, false_type>::value == true,
                  "1 true 2 false");
    static_assert(tp::or_<false_type, true_type, false_type>::value == true,
                  "1 true 2 false");
    static_assert(tp::or_<false_type, false_type, true_type>::value == true,
                  "1 true 2 false");
}

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

TEST(TypePack, apply) {
    using always_true = tp::always<true_type>;
/// @todo apply doesn't deal well with nullary function calls
#if 0
    static_assert(is_same<true_type, tp::apply<always_true>>::value,
                  "Applying always-true-type with no args");
#endif
    static_assert(tp::t_<tp::apply<tp::always<true_type>, int>>::value,
                  "Applying always-true-type with 1 arg");
    static_assert(tp::t_<tp::apply<tp::always<true_type>, int, char>>::value,
                  "Applying always-true-type with 2 args");
    static_assert(
        tp::t_<tp::apply<tp::always<true_type>, int, char, float>>::value,
        "Applying always-true-type with 3 args");
}

TEST(TypePack, fold) {
    /// using a dummy operation.
    using always_true = tp::always<true_type>;
    static_assert(
        tp::fold<mylist, true_type, always_true>::value,
        "turning everything into true and applying AND should be true");
}

TEST(TypePack, transform) {
    /// using a dummy transform.
    using always_true = tp::always<true_type>;
    static_assert(
        tp::fold<tp::transform<mylist, always_true>, true_type,
                 tp::quote<tp::and_>>::value,
        "turning everything into true and applying AND should be true");
    static_assert(
        tp::apply_list<tp::quote<tp::and_>,
                       tp::transform<mylist, always_true>>::value,
        "turning everything into true and applying AND should be true");

    static_assert(tp::size<tp::transform<mylist, always_true>>::value ==
                      mylist_len,
                  "Transform shouldn't change size");
}

TEST(TypePack, contains) {
    static_assert(tp::contains<mylist, myelt1>::value == true,
                  "list should contain its first element");
    static_assert(tp::contains<mylist, myelt2>::value == true,
                  "list should contain its second element");
    static_assert(tp::contains<mylist, myelt3>::value == true,
                  "list should contain its third element");

    static_assert(tp::contains<mylist, std::string>::value == false,
                  "list doesn't contain string");
}
