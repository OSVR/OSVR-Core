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
#include <osvr/Util/ContainerWrapper.h>

// Library/third-party includes
#include "gtest/gtest.h"

// Standard includes
#include <type_traits>
#include <string>
#include <vector>
using std::vector;
using std::string;
using std::is_same;
using osvr::util::ContainerWrapper;
namespace policies = osvr::util::container_policies;

typedef vector<string> svec;

template <typename Container, typename... Args>
class MyTestContainerWrapper : public ContainerWrapper<Container, Args...> {
  public:
    using Base = ContainerWrapper<Container, Args...>;
    using value_type = typename Base::value_type;
    void push_back(value_type const &newVal) {
        Base::container().push_back(newVal);
    }
};

static_assert(
    is_same<ContainerWrapper<svec, policies::const_iterators, policies::size>,
            ContainerWrapper<svec, policies::size,
                             policies::const_iterators>>::value,
    "Order of arguments shouldn't matter");
static_assert(is_same<ContainerWrapper<svec, policies::const_iterators>,
                      ContainerWrapper<svec, policies::const_iterators,
                                       policies::const_iterators>>::value,
              "Duplicated arguments shouldn't matter");

static_assert(is_same<ContainerWrapper<svec, policies::iterators,
                                       policies::const_iterators>,
                      ContainerWrapper<svec, policies::iterators>>::value,
              "iterators implies const_iterators");

TEST(ContainerWrapper, constructWithVariedArguments) {
    ASSERT_NO_THROW((MyTestContainerWrapper<svec>{}));
    ASSERT_NO_THROW(
        (MyTestContainerWrapper<svec, policies::const_iterators>{}));
    ASSERT_NO_THROW((MyTestContainerWrapper<svec, policies::iterators>{}));

    ASSERT_NO_THROW((MyTestContainerWrapper<svec, policies::size>{}));
    ASSERT_NO_THROW((MyTestContainerWrapper<svec, policies::size,
                                            policies::const_iterators>{}));
    ASSERT_NO_THROW(
        (MyTestContainerWrapper<svec, policies::size, policies::iterators>{}));
}
