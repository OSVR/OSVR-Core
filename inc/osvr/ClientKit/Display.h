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

#ifndef INCLUDED_Display_h_GUID_E15D0209_6784_4CBF_4F89_788E7A4B440C
#define INCLUDED_Display_h_GUID_E15D0209_6784_4CBF_4F89_788E7A4B440C

// Internal Includes
#include <osvr/ClientKit/DisplayC.h>
#include <osvr/ClientKit/Context.h>
#include <osvr/Util/SharedPtr.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace clientkit {
    typedef shared_ptr<::OSVR_DisplayConfigObject> DisplayConfigPtr;
    inline DisplayConfigPtr getDisplay(ClientContext &ctx) {
        ::OSVR_DisplayConfig cfg;
        osvrClientGetDisplay(ctx.get(), &cfg);
        DisplayConfigPtr ret(cfg, &::osvrClientFreeDisplay);
        return ret;
    }
} // namespace clientkit
} // namespace osvr
#endif // INCLUDED_Display_h_GUID_E15D0209_6784_4CBF_4F89_788E7A4B440C
