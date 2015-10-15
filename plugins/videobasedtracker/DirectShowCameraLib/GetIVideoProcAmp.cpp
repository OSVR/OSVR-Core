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
#include "GetIVideoProcAmp.h"

// Library/third-party includes
// - none

// Standard includes
#ifdef OSVR_HAVE_IVIDEOPROCAMP
#include <strmif.h>
#include <vidcap.h>  // for IKsTopologyInfo
#include <ksmedia.h> // for KSNODETYPE_VIDEO_PROCESSING

comutils::Ptr<IVideoProcAmp> getIVideoProcAmp(IBaseFilter &filter) {
    auto ret = comutils::Ptr<IVideoProcAmp>{};

    auto ksTopoInfo = comutils::Ptr<IKsTopologyInfo>{};
    filter.QueryInterface(IID_IKsTopologyInfo, AttachPtr(ksTopoInfo));
    if (!ksTopoInfo) {
        std::cout << "directx_camera_server: Couldn't get IKsTopologyInfo"
                  << std::endl;
        return ret;
    }
    auto numNodes = DWORD{0};
    ksTopoInfo->get_NumNodes(&numNodes);

    std::cout << "directx_camera_server: has " << numNodes << " nodes"
              << std::endl;
    for (DWORD i = 0; i < numNodes; ++i) {
        GUID nodeType;
        ksTopoInfo->get_NodeType(i, &nodeType);
        if (nodeType == KSNODETYPE_VIDEO_PROCESSING) {
            std::cout
                << "directx_camera_server: node has video processing type: "
                << i << std::endl;
            ksTopoInfo->CreateNodeInstance(i, IID_IVideoProcAmp,
                                           AttachPtr(ret));
            if (ret) {
                return ret;
            }
        }
    }
    return ret;
}
#endif // OSVR_HAVE_IVIDEOPROCAMP
