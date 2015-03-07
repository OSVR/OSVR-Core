/** @file
    @brief Header wrapping <boost/type_traits/is_copy_constructible.hpp>
    for when we just use it with static assertions or other non-critical
    uses, since it was added in 1.55.0.

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

#ifndef INCLUDED_BoostIsCopyConstructible_h_GUID_261BD733_2559_4586_CDEF_C551ECB87C4B
#define INCLUDED_BoostIsCopyConstructible_h_GUID_261BD733_2559_4586_CDEF_C551ECB87C4B

#include <boost/version.hpp>
#if BOOST_VERSION >= 105500
#define OSVR_HAVE_BOOST_IS_COPY_CONSTRUCTIBLE
#include <boost/type_traits/is_copy_constructible.hpp>
#endif

#endif // INCLUDED_BoostIsCopyConstructible_h_GUID_261BD733_2559_4586_CDEF_C551ECB87C4B
