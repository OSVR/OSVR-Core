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

#ifndef INCLUDED_DirectShowHDKCameraFactory_h_GUID_383B0AE1_8BD2_4B9D_5176_ABEFEF534DBE
#define INCLUDED_DirectShowHDKCameraFactory_h_GUID_383B0AE1_8BD2_4B9D_5176_ABEFEF534DBE

// Internal Includes
#include "directx_camera_server.h"

// Library/third-party includes
// - none

// Standard includes
#include <memory>
#include <iostream>

#include <ks.h>
#include <ksmedia.h>
#include <ksproxy.h>

// On the HDK IR camera, this actually puts the camera into the correct
// high-gain mode and has apparently nothing to do with powerline frequency.
inline void setPowerlineFrequencyTo50(IBaseFilter &filter) {
    auto ksPropSet = comutils::Ptr<IKsPropertySet>{};
    filter.QueryInterface(__uuidof(IKsPropertySet), AttachPtr(ksPropSet));
    if (!ksPropSet) {
        // std::cout << "Couldn't get ksPropSet" << std::endl;
        return;
    }
    auto prop = KSPROPERTY{};
    prop.Set = GUID{STATICGUIDOF(
        PROPSETID_VIDCAP_VIDEOPROCAMP)}; // workaround undefined reference, no
                                         // need to link for one silly guid.
    prop.Id = KSPROPERTY_VIDEOPROCAMP_POWERLINE_FREQUENCY;

    auto supportType = DWORD{0};
    auto hr = ksPropSet->QuerySupported(prop.Set, prop.Id, &supportType);
    if (SUCCEEDED(hr) && (supportType & KSPROPERTY_SUPPORT_SET)) {
        // If we got here, we know we can set the "powerline" property
        // past this point MSDN was almost useless to a non-driver-writer
        // Chrome source code path below was a useful reference
        // chromium/src/media/capture/video/win/video_capture_device_win.cc
        static const long POWERLINE_DISABLED = 0;
        static const long POWERLINE_50HZ = 1;
        static const long POWERLINE_60HZ = 2;

        auto propData = KSPROPERTY_VIDEOPROCAMP_S{};
        propData.Property = prop;
        propData.Property.Flags = KSPROPERTY_TYPE_SET;

        propData.Value = POWERLINE_50HZ;
        propData.Flags = KSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL;

        hr = ksPropSet->Set(prop.Set, prop.Id, &propData, sizeof(propData),
                            &propData, sizeof(propData));

        if (FAILED(hr)) {
            std::cout
                << "Tried but failed to put the camera in high gain mode..."
                << std::endl;
        }
    }
}

inline std::unique_ptr<directx_camera_server> getDirectShowHDKCamera() {
    // This string begins the DevicePath provided by Windows for the HDK's
    // camera.
    static const auto HDK_CAMERA_PATH_PREFIX =
        "\\\\?\\usb#vid_0bda&pid_57e8&mi_00";
    auto ret = std::unique_ptr<directx_camera_server>{new directx_camera_server(
        HDK_CAMERA_PATH_PREFIX, &setPowerlineFrequencyTo50)};

    if (!ret->working()) {
        // If it didn't start up right, just ditch it now.
        ret.reset();
    }
    return ret;
}

#endif // INCLUDED_DirectShowHDKCameraFactory_h_GUID_383B0AE1_8BD2_4B9D_5176_ABEFEF534DBE
