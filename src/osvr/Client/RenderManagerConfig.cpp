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

// Internal Includes
#include <osvr/Client/RenderManagerConfig.h>
#include <osvr/Common/ClientContext.h>
#include <osvr/Util/Verbosity.h>
#include <osvr/Common/JSONHelpers.h>

// Library/third-party includes
#include <json/value.h>
#include <json/reader.h>

// Standard includes
#include <stdexcept>

namespace osvr {
namespace client {

    RenderManagerConfigPtr RenderManagerConfigFactory::createShared(OSVR_ClientContext ctx) {
        try {
            auto const configString = ctx->getStringParameter("/renderManagerConfig");
            RenderManagerConfigPtr cfg(new RenderManagerConfig(configString));
            return cfg;
        }
        catch (std::exception const &e) {
            OSVR_DEV_VERBOSE(
                "Couldn't create a render manager config internally! Exception: "
                << e.what());
            return RenderManagerConfigPtr{};
        }
        catch (...) {
            OSVR_DEV_VERBOSE("Couldn't create a render manager config internally! "
                "Unknown exception!");
            return RenderManagerConfigPtr{};
        }
    }

    RenderManagerConfig::RenderManagerConfig(const std::string& renderManagerConfig)
    {
        parse(renderManagerConfig);
    }

    void RenderManagerConfig::parse(const std::string& renderManagerConfig)
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
    }

    void RenderManagerConfig::print() const
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
      std::cout << "Enable time warp: " << m_enableTimeWarp << std::endl;
      std::cout << "Asynchronous time warp: " << m_asynchronousTimeWarp << std::endl;
      std::cout << "Max ms before vsync time warp: " << m_maxMSBeforeVsyncTimeWarp << std::endl;
      std::cout << "Render overfill factor: " << m_renderOverfillFactor << std::endl;
    }

    bool RenderManagerConfig::getDirectMode() const
    {
      return m_directMode;
    }

    uint32_t RenderManagerConfig::getDisplayIndex() const
    {
      return m_displayIndex;
    }

    bool RenderManagerConfig::getDirectHighPriority() const
    {
      return m_directHighPriority;
    }

    std::size_t RenderManagerConfig::getNumBuffers() const
    {
      return m_numBuffers;
    }

    bool RenderManagerConfig::getVerticalSync() const
    {
      return m_verticalSync;
    }

    bool RenderManagerConfig::getVerticalSyncBlockRendering() const
    {
      return m_verticalSyncBlockRendering;
    }

    std::string RenderManagerConfig::getWindowTitle() const
    {
      return m_windowTitle;
    }

    bool RenderManagerConfig::getWindowFullScreen() const
    {
      return m_windowFullScreen;
    }

    int32_t RenderManagerConfig::getWindowXPosition() const
    {
      return m_windowXPosition;
    }

    int32_t RenderManagerConfig::getWindowYPosition() const
    {
      return m_windowYPosition;
    }

    uint32_t RenderManagerConfig::getDisplayRotation() const
    {
      return m_displayRotation;
    }

    uint32_t RenderManagerConfig::getBitsPerColor() const
    {
      return m_bitsPerColor;
    }

    bool RenderManagerConfig::getEnableTimeWarp() const
    {
      return m_enableTimeWarp;
    }

    bool RenderManagerConfig::getAsynchronousTimeWarp() const
    {
      return m_asynchronousTimeWarp;
    }

    float RenderManagerConfig::getMaxMSBeforeVsyncTimeWarp() const
    {
      return m_maxMSBeforeVsyncTimeWarp;
    }

    float RenderManagerConfig::getRenderOverfillFactor() const
    {
      return m_renderOverfillFactor;
    }

} // namespace client
} // namespace osvr