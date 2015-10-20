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

// Required for DLL linkage on Windows
#include <osvr/Client/Export.h>
#include <osvr/Util/UniquePtr.h>
#include <osvr/Util/ClientOpaqueTypesC.h>

// Standard includes
#include <string>
#include <iostream>
#include <exception>
#include <vector>

namespace osvr {
namespace client {

    class RenderManagerConfigParseException : public std::exception {
    public:
        OSVR_CLIENT_EXPORT RenderManagerConfigParseException(const std::string& message) /*OSVR_NOEXCEPT*/ : std::exception(), m_message(message)
        {
            // do nothing
        }

        virtual const char OSVR_CLIENT_EXPORT *what() const /*OSVR_NOEXCEPT OSVR_OVERRIDE*/
        {
            return m_message.c_str();
        }

    private:
        const std::string m_message;
    };

    class RenderManagerConfig;
    typedef unique_ptr<RenderManagerConfig> RenderManagerConfigPtr;
    class RenderManagerConfigFactory {
    public:
        OSVR_CLIENT_EXPORT static RenderManagerConfigPtr
            create(OSVR_ClientContext ctx);
    };

    class RenderManagerConfig {
    public:
        OSVR_CLIENT_EXPORT RenderManagerConfig(const std::string& render_manager_config);
        OSVR_CLIENT_EXPORT RenderManagerConfig() {};

        void OSVR_CLIENT_EXPORT parse(const std::string& render_manager_config);
        void OSVR_CLIENT_EXPORT print() const;

        /// Read the property information.
        bool OSVR_CLIENT_EXPORT getDirectMode() const;
        unsigned OSVR_CLIENT_EXPORT getDisplayIndex() const;
        bool OSVR_CLIENT_EXPORT getDirectHighPriority() const;

        bool OSVR_CLIENT_EXPORT getEnableTimeWarp() const;
        bool OSVR_CLIENT_EXPORT getAsynchronousTimeWarp() const;
        float OSVR_CLIENT_EXPORT getMaxMSBeforeVsyncTimeWarp() const;
        float OSVR_CLIENT_EXPORT getRenderOverfillFactor() const;

        unsigned OSVR_CLIENT_EXPORT getNumBuffers() const;
        bool OSVR_CLIENT_EXPORT getVerticalSync() const;
        bool OSVR_CLIENT_EXPORT getVerticalSyncBlockRendering() const;

        std::string OSVR_CLIENT_EXPORT getWindowTitle() const;
        bool OSVR_CLIENT_EXPORT getWindowFullScreen() const;
        int OSVR_CLIENT_EXPORT getWindowXPosition() const;
        int OSVR_CLIENT_EXPORT getWindowYPosition() const;
        unsigned OSVR_CLIENT_EXPORT getDisplayRotation() const;
        unsigned OSVR_CLIENT_EXPORT getBitsPerColor() const;

    private:
        bool m_directMode;
        unsigned m_displayIndex;
        bool m_directHighPriority;
        unsigned m_numBuffers;
        bool m_verticalSync;
        bool m_verticalSyncBlockRendering;

        std::string m_windowTitle;
        bool m_windowFullScreen;
        int m_windowXPosition;
        int m_windowYPosition;
        unsigned m_displayRotation;
        unsigned m_bitsPerColor;

        bool m_enableTimeWarp;
        bool m_asynchronousTimeWarp;
        float m_maxMSBeforeVsyncTimeWarp;
        float m_renderOverfillFactor;
    };

} // namespace client
} // namespace osvr

#endif // INCLUDED_RenderManagerConfig_h_GUID_C8DA5781_5B6C_454A_B4FF_1DB50CBE3479