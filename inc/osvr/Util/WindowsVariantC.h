/** @file
    @brief Header for determining what "WinAPI Family Partition" we're in, in a
   simple way.

    Must be c-safe!

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

/*
// Copyright 2015 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
*/

#ifndef INCLUDED_WindowsVariantC_h_GUID_968C6480_6D3E_4DA3_87C7_C5C2F8E4E4BA
#define INCLUDED_WindowsVariantC_h_GUID_968C6480_6D3E_4DA3_87C7_C5C2F8E4E4BA

/* Internal Includes */
#include <osvr/Util/PlatformConfig.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

#ifdef OSVR_WINDOWS

/* Visual Studio 2012 or newer, not using the vXX0_xp toolset*/
#if defined(_MSC_VER) && (_MSC_VER >= 1700) &&                                 \
    (!defined(_USING_V110_SDK71_) || !_USING_V110_SDK71_)
#include <winapifamily.h>

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
/* Windows desktop */
#define OSVR_WINDOWS_DESKTOP
#define OSVR_WINDOWS_PARTITION_KNOWN

#elif WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)
/* Windows RT/Modern SDK/Store apps */
#define OSVR_WINDOWS_STORE
#define OSVR_WINDOWS_PARTITION_KNOWN

#elif WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_PHONE_APP)
/* Windows Phone */
#define OSVR_WINDOWS_PHONE
#define OSVR_WINDOWS_PARTITION_KNOWN
#endif

#endif /* new msvc */

#ifndef OSVR_WINDOWS_PARTITION_KNOWN
/* Fallback: assume Windows desktop */
#define OSVR_WINDOWS_DESKTOP
#endif

#endif /* OSVR_WINDOWS */

#endif
