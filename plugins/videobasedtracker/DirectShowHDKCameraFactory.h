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
#include "GetIVideoProcAmp.h"

// Library/third-party includes
// - none

// Standard includes
#include <memory>

#ifdef OSVR_HAVE_IVIDEOPROCAMP
#include <iostream>
#include <vidcap.h> // for IVideoProcAmp

static void setPowerlineFrequencyTo50(IBaseFilter &filter) {
    auto procAmp = getIVideoProcAmp(filter);
    if (!procAmp) {
        std::cout << "directx_camera_server: Couldn't get IVideoProcAmp"
                  << std::endl;
        return;
    }
    static const long POWERLINE_DISABLED = 0;
    static const long POWERLINE_50HZ = 1;
    static const long POWERLINE_60HZ = 2;
    auto hr = procAmp->put_PowerlineFrequency(POWERLINE_50HZ,
                                              VideoProcAmp_Flags_Manual);
    if (SUCCEEDED(hr)) {
        std::cout << "directx_camera_server: Successfully set powerline "
                     "frequency to 50Hz"
                  << std::endl;
    } else {
        std::cout << "directx_camera_server: Almost, but couldn't, set "
                     "powerline frequency to 50Hz"
                  << std::endl;
    }
}
#else
static void setPowerlineFrequencyTo50(IBaseFilter &) {
    // no-op, header missing
}
#endif

std::unique_ptr<directx_camera_server> getDirectShowHDKCamera() {
    // This string begins the DevicePath provided by Windows for the HDK's
    // camera.
    static const auto HDK_CAMERA_PATH_PREFIX =
        "\\\\?\\usb#vid_0bda&pid_57e8&mi_00";
    auto ret = std::unique_ptr<directx_camera_server>{
        new directx_camera_server(HDK_CAMERA_PATH_PREFIX)};
    if (!ret->working()) {
        ret.reset();
    }
    return ret;
}

#endif // INCLUDED_DirectShowHDKCameraFactory_h_GUID_383B0AE1_8BD2_4B9D_5176_ABEFEF534DBE
