/** @file
    @brief Header for std::shared_ptr-based management of Deletables

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

#ifndef INCLUDED_StdDeletable_h_GUID_2C03AF21_E4B8_44B3_0F77_630666063711
#define INCLUDED_StdDeletable_h_GUID_2C03AF21_E4B8_44B3_0F77_630666063711

// Internal Includes
#include <osvr/Util/Deletable.h>
#include <osvr/Util/SharedPtr.h>

// Library/third-party includes
// - none

// Standard includes
#include <vector>

namespace osvr {
namespace util {
    /// @brief Shared-ownership smart pointer to a Deletable
    typedef shared_ptr<Deletable> DeletablePtr;
    /// @brief Vector of shared-ownership smart pointers
    typedef ::std::vector<DeletablePtr> DeletableList;
} // namespace util
} // namespace osvr
#endif // INCLUDED_StdDeletable_h_GUID_2C03AF21_E4B8_44B3_0F77_630666063711
