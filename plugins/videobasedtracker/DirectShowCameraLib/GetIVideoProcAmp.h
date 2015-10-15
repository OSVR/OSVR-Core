/** @file
    @brief Header

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

#ifndef INCLUDED_GetIVideoProcAmp_h_GUID_F35BD30E_FB93_45BF_9957_8E7F69290E46
#define INCLUDED_GetIVideoProcAmp_h_GUID_F35BD30E_FB93_45BF_9957_8E7F69290E46

// Internal Includes
#include "comutils/ComPtr.h"

// Library/third-party includes
// - none

// Standard includes
// - none

#ifdef _MSC_VER
/// Apparently no vidcap header in MinGW64 yet.
/// @todo actually test for vidcap.h
#define OSVR_HAVE_IVIDEOPROCAMP

struct IBaseFilter;
struct IVideoProcAmp;
comutils::Ptr<IVideoProcAmp> getIVideoProcAmp(IBaseFilter &filter);
#endif

#endif // INCLUDED_GetIVideoProcAmp_h_GUID_F35BD30E_FB93_45BF_9957_8E7F69290E46
