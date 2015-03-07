/** @file
    @brief Header for boost::shared_ptr-based management of Deletables

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

#ifndef INCLUDED_Deletable_h_GUID_66D6C45A_7AB7_4198_F8EE_202F0FCE682C
#define INCLUDED_Deletable_h_GUID_66D6C45A_7AB7_4198_F8EE_202F0FCE682C

// Internal Includes
#include <osvr/Util/Deletable.h>

// Library/third-party includes
#include <boost/shared_ptr.hpp>

// Standard includes
#include <vector>

namespace osvr {
namespace util {
    namespace boost_util {
        /// @brief Shared-ownership smart pointer to a Deletable, using Boost's
        /// shared pointers.
        typedef ::boost::shared_ptr<Deletable> DeletablePtr;
        /// @brief Vector of shared-ownership smart pointers
        typedef ::std::vector<DeletablePtr> DeletableList;
    } // namespace boost_util
} // namespace util
} // namespace osvr

#endif // INCLUDED_Deletable_h_GUID_66D6C45A_7AB7_4198_F8EE_202F0FCE682C
