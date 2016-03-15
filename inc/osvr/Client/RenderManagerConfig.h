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
#include <osvr/Common/ClientContext.h>
#include <osvr/Common/JSONHelpers.h>

// Library/third-party includes
#include <json/value.h>
#include <json/reader.h>

// Standard includes
#include <string>
#include <iostream>
#include <exception>
#include <vector>
#include <stdexcept>

namespace osvr {
    namespace client {

        class RenderManagerConfig {
        public:

            inline RenderManagerConfig(const std::string &renderManagerConfig)
            {
                parse(renderManagerConfig);
            }

            inline RenderManagerConfig() {}

            inline void parse(const std::string &renderManagerConfig)
            {
                Json::Reader reader;
                Json::Value root;
                if (!reader.parse(renderManagerConfig, root, false)) {
                    throw std::runtime_error("RenderManagerConfig::parse - failed to parse render manager config string. Invalid JSON value?");
                }

                auto &params = root["renderManagerConfig"];
                m_directMode = params["directModeEnabled"].asBool();
                m_displayIndex = params["directDisplayIndex"].asUInt();
                m_directHighPriority = params["directHighPriorityEnabled"].asBool();
                m_numBuffers = params["numBuffers"].asUInt();
                m_verticalSync = params["verticalSyncEnabled"].asBool();
                m_verticalSyncBlockRendering = params["verticalSyncBlockRenderingEnabled"].asBool();
                m_renderOverfillFactor = params["renderOverfillFactor"].asFloat();
                m_renderOversampleFactor = params.get("renderOversampleFactor", 1).asFloat();

                // window
                {
                    auto &window = params["window"];
                    m_windowTitle = window["title"].asString();
                    m_windowFullScreen = window["fullScreenEnabled"].asBool();
                    m_windowXPosition = window["xPosition"].asInt();
                    m_windowYPosition = window["yPosition"].asInt();
                }

                // display
                {
                    auto display = params["display"];
                    m_displayRotation = display["rotation"].asUInt();
                    m_bitsPerColor = display["bitsPerColor"].asUInt();
                }

                // time warp
                {
                    auto &timeWarp = params["timeWarp"];
                    m_enableTimeWarp = timeWarp["enabled"].asBool();
                    m_asynchronousTimeWarp = timeWarp["asynchronous"].asBool();
                    m_maxMSBeforeVsyncTimeWarp = timeWarp["maxMsBeforeVSync"].asFloat();
                }

                // prediction
                {
                  auto prediction = params["prediction"];
                  m_predictEnabled = prediction["enabled"].asBool();
                  m_predictStaticDelayMS = prediction["staticDelayMS"].asFloat();
                  m_predictLeftEyeDelayMS = prediction["leftEyeDelayMS"].asFloat();
                  m_predictRightEyeDelayMS = prediction["rightEyeDelayMS"].asFloat();
                  m_predictLocalTimeOverride = prediction["localTimeOverride"].asBool();
                }
            }

            inline void print() const
            {
                std::cout << "Direct mode: " << m_directMode << std::endl;
                std::cout << "Direct mode display index: " << m_displayIndex << std::endl;
                std::cout << "Number of buffers: " << m_numBuffers << std::endl;
                std::cout << "Vertical sync: " << m_verticalSync << std::endl;
                std::cout << "Vertical sync block rendering: " << m_verticalSyncBlockRendering << std::endl;
                std::cout << "Window Title: " << m_windowTitle << std::endl;
                std::cout << "Window full screen: " << m_windowFullScreen << std::endl;
                std::cout << "Window X Position: " << m_windowXPosition << std::endl;
                std::cout << "Window Y Position: " << m_windowYPosition << std::endl;
                std::cout << "Display rotation: " << m_displayRotation << std::endl;
                std::cout << "Bits per color: " << m_bitsPerColor << std::endl;
                std::cout << "Prediction enabled: " << m_predictEnabled << std::endl;
                std::cout << "Static delay (ms): " << m_predictStaticDelayMS << std::endl;
                std::cout << "Left eye delay (ms): " << m_predictLeftEyeDelayMS << std::endl;
                std::cout << "Right eye delay (ms): " << m_predictRightEyeDelayMS << std::endl;
                std::cout << "Prediction local time override: " << m_predictLocalTimeOverride << std::endl;
                std::cout << "Enable time warp: " << m_enableTimeWarp << std::endl;
                std::cout << "Asynchronous time warp: " << m_asynchronousTimeWarp << std::endl;
                std::cout << "Max ms before vsync time warp: " << m_maxMSBeforeVsyncTimeWarp << std::endl;
                std::cout << "Render overfill factor: " << m_renderOverfillFactor << std::endl;
                std::cout << "Render oversample factor: " << m_renderOversampleFactor << std::endl;
            }

            /// Read the property information.
            inline bool getDirectMode() const
            {
                return m_directMode;
            }

            inline uint32_t getDisplayIndex() const
            {
                return m_displayIndex;
            }

            inline bool getDirectHighPriority() const
            {
                return m_directHighPriority;
            }

            inline bool getEnableTimeWarp() const
            {
                return m_enableTimeWarp;
            }

            inline bool getAsynchronousTimeWarp() const
            {
                return m_asynchronousTimeWarp;
            }

            inline float getMaxMSBeforeVsyncTimeWarp() const
            {
                return m_maxMSBeforeVsyncTimeWarp;
            }

            inline float getRenderOverfillFactor() const
            {
                return m_renderOverfillFactor;
            }

            inline float getRenderOversampleFactor() const
            {
              return m_renderOversampleFactor;
            }

            inline std::size_t getNumBuffers() const
            {
                return m_numBuffers;
            }

            inline bool getVerticalSync() const
            {
                return m_verticalSync;
            }

            inline bool getVerticalSyncBlockRendering() const
            {
                return m_verticalSyncBlockRendering;
            }

            inline std::string getWindowTitle() const
            {
                return m_windowTitle;
            }

            inline bool getWindowFullScreen() const
            {
                return m_windowFullScreen;
            }

            inline int32_t getWindowXPosition() const
            {
                return m_windowXPosition;
            }

            inline int32_t getWindowYPosition() const
            {
                return m_windowYPosition;
            }

            inline uint32_t getDisplayRotation() const
            {
                return m_displayRotation;
            }

            inline uint32_t getBitsPerColor() const
            {
                return m_bitsPerColor;
            }

            inline bool getclientPredictionEnabled() const
            {
              return m_predictEnabled;
            }

            inline float getStaticDelayMS() const
            {
              return m_predictStaticDelayMS;
            }

            inline float getLeftEyeDelayMS() const
            {
              return m_predictLeftEyeDelayMS;
            }

            inline float getRightEyeDelayMS() const
            {
              return m_predictRightEyeDelayMS;
            }

            inline bool getclientPredictionLocalTimeOverride() const
            {
              return m_predictLocalTimeOverride;
            }

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
            bool m_predictEnabled;
            float m_predictStaticDelayMS;
            float m_predictLeftEyeDelayMS;
            float m_predictRightEyeDelayMS;
            bool m_predictLocalTimeOverride;

            bool m_enableTimeWarp;
            bool m_asynchronousTimeWarp;
            float m_maxMSBeforeVsyncTimeWarp;
            float m_renderOverfillFactor;
            float m_renderOversampleFactor;
        };

        typedef std::shared_ptr<RenderManagerConfig> RenderManagerConfigPtr;

        class RenderManagerConfigFactory {
        public:
            inline static RenderManagerConfigPtr createShared(OSVR_ClientContext ctx)
            {
                try {
                    auto const configString = ctx->getStringParameter("/renderManagerConfig");
                    RenderManagerConfigPtr cfg(new RenderManagerConfig(configString));
                    return cfg;
                }
                catch (std::exception const &e) {
                    std::cerr <<
                        "Couldn't create a render manager config internally! Exception: "
                        << e.what() << std::endl;
                    return RenderManagerConfigPtr{};
                }
                catch (...) {
                    std::cerr << "Couldn't create a render manager config internally! "
                        "Unknown exception!" << std::endl;
                    return RenderManagerConfigPtr{};
                }
            }
        };

    } // namespace client
} // namespace osvr

#endif // INCLUDED_RenderManagerConfig_h_GUID_C8DA5781_5B6C_454A_B4FF_1DB50CBE3479
