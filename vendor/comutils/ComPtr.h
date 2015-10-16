/** @file
    @brief Header with a template alias for the desired COM smart pointer.

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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_ComPtr_h_GUID_2738ADED_FE2F_4FBE_92BF_0D54DE8FFF08
#define INCLUDED_ComPtr_h_GUID_2738ADED_FE2F_4FBE_92BF_0D54DE8FFF08

// Internal Includes
// - none

// Library/third-party includes
#include <boost/intrusive_ptr.hpp>
#include <intrusive_ptr_COM.h>

// Standard includes
// - none

namespace comutils {
/// @brief Template alias for our desired COM smart pointer.
template <typename T> using Ptr = boost::intrusive_ptr<T>;
} // namespace comutils

#endif // INCLUDED_ComPtr_h_GUID_2738ADED_FE2F_4FBE_92BF_0D54DE8FFF08
