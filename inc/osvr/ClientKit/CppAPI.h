/** @file
    @brief Header configuring some general properties of the header-only C++ API
   provided by ClientKit.

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

#ifndef INCLUDED_CppAPI_h_GUID_C033B389_3FF4_44AF_AA03_3421F95748AB
#define INCLUDED_CppAPI_h_GUID_C033B389_3FF4_44AF_AA03_3421F95748AB

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
// - none

#ifndef OSVR_CLIENTKIT_HAVE_TEMPLATE_UNIVERSAL_REF
#if defined(_MSC_VER) && (_MSC_VER >= 1800)
#define OSVR_CLIENTKIT_HAVE_TEMPLATE_UNIVERSAL_REF
#elif defined(__GXX_EXPERIMENTAL_CXX0X) || __cplusplus >= 201103L
#define OSVR_CLIENTKIT_HAVE_TEMPLATE_UNIVERSAL_REF
#endif // conditions when we know we have template universal ref.
#endif

/// @brief Define if your compiler supports && in a type-deduced context.
#ifdef OSVR_CLIENTKIT_HAVE_TEMPLATE_UNIVERSAL_REF
#define OSVR_CLIENTKIT_FUNCTOR_REF &&
#else
#define OSVR_CLIENTKIT_FUNCTOR_REF
#endif

#endif // INCLUDED_CppAPI_h_GUID_C033B389_3FF4_44AF_AA03_3421F95748AB
