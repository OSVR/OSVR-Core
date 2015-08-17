/** @file
    @brief Header to bring shared_ptr into the ::osvr namespace.

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

#ifndef INCLUDED_SharedPtr_h_GUID_E9C5BC8D_7D3A_4896_1552_6F4F5292783C
#define INCLUDED_SharedPtr_h_GUID_E9C5BC8D_7D3A_4896_1552_6F4F5292783C

#if defined(OSVR_SHAREDPTR_USE_BOOST) && defined(OSVR_SHAREDPTR_USE_STD)
#error "Can't use both boost and std shared_ptr"
#endif

#if !defined(OSVR_SHAREDPTR_USE_BOOST) && !defined(OSVR_SHAREDPTR_USE_STD)

#if defined(_MSC_VER)
#if (_MSC_VER < 1600)
// no std::shared_ptr before VS 2010
#define OSVR_SHAREDPTR_USE_BOOST
#else
#define OSVR_SHAREDPTR_USE_STD
#endif

#elif defined(__GXX_EXPERIMENTAL_CXX0X) || __cplusplus >= 201103L
// GCC and friends in the right mode
#define OSVR_SHAREDPTR_USE_STD

#else
// Couldn't guess if we have std::shared_ptr, so assume not.
#define OSVR_SHAREDPTR_USE_BOOST
#endif // guessing
#endif // !defined(OSVR_SHAREDPTR_USE_BOOST) && !defined(OSVR_SHAREDPTR_USE_STD)

#ifdef OSVR_SHAREDPTR_USE_BOOST
#include <boost/shared_ptr.hpp>
namespace osvr {
using boost::shared_ptr;
using boost::weak_ptr;
using boost::make_shared;
using boost::enable_shared_from_this;
} // namespace osvr

#else
#include <memory>

namespace osvr {
using std::shared_ptr;
using std::weak_ptr;
using std::make_shared;
using std::enable_shared_from_this;
} // namespace osvr
#endif

#endif // INCLUDED_SharedPtr_h_GUID_E9C5BC8D_7D3A_4896_1552_6F4F5292783C
