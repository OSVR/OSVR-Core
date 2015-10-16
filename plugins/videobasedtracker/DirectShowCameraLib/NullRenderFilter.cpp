/** @file
    @brief Implementation

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

// Internal Includes
#include "NullRenderFilter.h"

// Library/third-party includes
// - none

// Standard includes
// - none

extern "C" const CLSID CLSID_NullRenderer;

comutils::Ptr<IBaseFilter> createNullRenderFilter() {

    auto pNullRender = comutils::Ptr<IBaseFilter>{};
    CoCreateInstance(CLSID_NullRenderer, nullptr, CLSCTX_INPROC_SERVER,
                     IID_IBaseFilter, AttachPtr(pNullRender));
    return pNullRender;
}
