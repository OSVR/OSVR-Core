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

#ifndef INCLUDED_GetPin_h_GUID_5AABE0CF_8981_4785_0078_147205EEFABF
#define INCLUDED_GetPin_h_GUID_5AABE0CF_8981_4785_0078_147205EEFABF

// Internal Includes
#include "comutils/ComPtr.h"

// Library/third-party includes
#include <strmif.h>

// Standard includes
// - none

/// Helper function to get a pin of a particular direction.
inline comutils::Ptr<IPin> GetPin(IBaseFilter &pFilter,
                                  PIN_DIRECTION const PinDir) {
    auto pEnum = comutils::Ptr<IEnumPins>{};
    pFilter.EnumPins(AttachPtr(pEnum));
    auto pPin = comutils::Ptr<IPin>{};
    while (pEnum->Next(1, AttachPtr(pPin), nullptr) == S_OK) {
        PIN_DIRECTION PinDirThis;
        pPin->QueryDirection(&PinDirThis);
        if (PinDir == PinDirThis) {
            return pPin;
        }
    }
    return comutils::Ptr<IPin>{};
}

template <typename IfaceType>
inline comutils::Ptr<IfaceType>
GetPinInterface(ICaptureGraphBuilder2 &builder, IBaseFilter &src,
                const GUID *pinCategory = nullptr,
                const GUID *mediaType = nullptr) {
    comutils::Ptr<IfaceType> ret;
    builder.FindInterface(pinCategory, mediaType, &src, __uuidof(IfaceType),
                          AttachPtr(ret));
    return ret;
}

template <typename IfaceType>
inline comutils::Ptr<IfaceType>
GetVideoCapturePinInterface(ICaptureGraphBuilder2 &builder, IBaseFilter &src) {
    return GetPinInterface<IfaceType>(builder, src, &PIN_CATEGORY_CAPTURE,
                                      &MEDIATYPE_Video);
}

#endif // INCLUDED_GetPin_h_GUID_5AABE0CF_8981_4785_0078_147205EEFABF
