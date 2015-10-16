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

#ifndef INCLUDED_ConnectTwoFilters_h_GUID_2F465D2A_B6CB_4CBD_E976_22BB9613AC94
#define INCLUDED_ConnectTwoFilters_h_GUID_2F465D2A_B6CB_4CBD_E976_22BB9613AC94

// Internal Includes
#include "GetPin.h"

// Library/third-party includes
#include <strmif.h>

// Standard includes
// - none

/// Helper function for connecting (the first) output pin on one filter to (the
/// first) input pin on another filter.
inline HRESULT ConnectTwoFilters(IGraphBuilder &pGraph, IBaseFilter &pFirst,
                                 IBaseFilter &pSecond) {
    auto pOut = GetPin(pFirst, PINDIR_OUTPUT);
    auto pIn = GetPin(pSecond, PINDIR_INPUT);
    return pGraph.Connect(pOut.get(), pIn.get());
}

#endif // INCLUDED_ConnectTwoFilters_h_GUID_2F465D2A_B6CB_4CBD_E976_22BB9613AC94
