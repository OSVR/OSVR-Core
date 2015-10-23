/** @file
    @brief OSVR rendermanager configuration

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com>

    */

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

#ifndef INCLUDED_RenderManagerConfig_h_GUID_C8DA5781_5B6C_454A_B4FF_1DB50CBE3479
#define INCLUDED_RenderManagerConfig_h_GUID_C8DA5781_5B6C_454A_B4FF_1DB50CBE3479

// Internal Includes
#include <osvr/Client/Export.h>
#include <osvr/Util/UniquePtr.h>
#include <osvr/Util/ClientOpaqueTypesC.h>

// Library/third-party includes
// - none

// Standard includes
#include <string>
#include <iostream>
#include <exception>
#include <vector>

namespace osvr {
namespace client {

    class RenderManagerConfig;
    typedef std::shared_ptr<RenderManagerConfig> RenderManagerConfigPtr;
    class RenderManagerConfigFactory {
    public:
        OSVR_CLIENT_EXPORT static RenderManagerConfigPtr
            createShared(OSVR_ClientContext ctx);
    };

    class RenderManagerConfig {
    public:
        OSVR_CLIENT_EXPORT RenderManagerConfig(const std::string& renderManagerConfig);
        OSVR_CLIENT_EXPORT RenderManagerConfig() {};

        OSVR_CLIENT_EXPORT void parse(const std::string& renderManagerConfig);
        OSVR_CLIENT_EXPORT void print() const;

        /// Read the property information.
        OSVR_CLIENT_EXPORT bool getDirectMode() const;
        OSVR_CLIENT_EXPORT uint32_t getDisplayIndex() const;
        OSVR_CLIENT_EXPORT bool getDirectHighPriority() const;

        OSVR_CLIENT_EXPORT bool getEnableTimeWarp() const;
        OSVR_CLIENT_EXPORT bool getAsynchronousTimeWarp() const;
        OSVR_CLIENT_EXPORT float getMaxMSBeforeVsyncTimeWarp() const;
        OSVR_CLIENT_EXPORT float getRenderOverfillFactor() const;

        OSVR_CLIENT_EXPORT std::size_t getNumBuffers() const;
        OSVR_CLIENT_EXPORT bool getVerticalSync() const;
        OSVR_CLIENT_EXPORT bool getVerticalSyncBlockRendering() const;

        OSVR_CLIENT_EXPORT std::string getWindowTitle() const;
        OSVR_CLIENT_EXPORT bool getWindowFullScreen() const;
        OSVR_CLIENT_EXPORT int32_t getWindowXPosition() const;
        OSVR_CLIENT_EXPORT int32_t getWindowYPosition() const;
        OSVR_CLIENT_EXPORT uint32_t getDisplayRotation() const;
        OSVR_CLIENT_EXPORT uint32_t getBitsPerColor() const;

    private:
        bool m_directMode;
        uint32_t m_displayIndex;
        bool m_directHighPriority;
        std::size_t m_numBuffers;
        bool m_verticalSync;
        bool m_verticalSyncBlockRendering;

        std::string m_windowTitle;
        bool m_windowFullScreen;
        int32_t m_windowXPosition;
        int32_t m_windowYPosition;
        uint32_t m_displayRotation;
        uint32_t m_bitsPerColor;

        bool m_enableTimeWarp;
        bool m_asynchronousTimeWarp;
        float m_maxMSBeforeVsyncTimeWarp;
        float m_renderOverfillFactor;
    };

} // namespace client
} // namespace osvr

#endif // INCLUDED_RenderManagerConfig_h_GUID_C8DA5781_5B6C_454A_B4FF_1DB50CBE3479