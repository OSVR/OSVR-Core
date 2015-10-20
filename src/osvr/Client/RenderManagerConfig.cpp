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

// Library/third-party includes
#include <json/value.h>
#include <json/reader.h>

namespace osvr {
namespace client {
RenderManagerConfig::RenderManagerConfig(const std::string& render_manager_config)
{
    parse(render_manager_config);
}

void RenderManagerConfig::parse(const std::string& render_manager_config)
{
  Json::Reader reader;
  Json::Value root;
  reader.parse(render_manager_config, root, false);

  m_directMode = root["render_manager_parameters"]["direct_mode"].asBool();
  m_displayIndex = root["render_manager_parameters"]["direct_display_index"].asUInt();
  m_directHighPriority = root["render_manager_parameters"]["direct_high_priority"].asBool();

  m_numBuffers = root["render_manager_parameters"]["num_buffers"].asUInt();
  m_verticalSync = root["render_manager_parameters"]["vertical_sync"].asBool();
  m_verticalSyncBlockRendering = root["render_manager_parameters"]
    ["vertical_sync_block_rendering"].asBool();
  m_windowTitle = root["render_manager_parameters"]["window_title"].asString();
  m_windowFullScreen = root["render_manager_parameters"]["window_full_screen"].asBool();
  m_windowXPosition = root["render_manager_parameters"]["window_x_position"].asInt();
  m_windowYPosition = root["render_manager_parameters"]["window_y_position"].asInt();
  m_displayRotation = root["render_manager_parameters"]["display_rotation"].asUInt();
  m_bitsPerColor = root["render_manager_parameters"]["bits_per_color"].asBool();

  m_enableTimeWarp = root["render_manager_parameters"]["time_warp"]["enable"].asBool();
  m_asynchronousTimeWarp = root["render_manager_parameters"]["time_warp"]["asynchronous_time_warp"].asBool();
  m_maxMSBeforeVsyncTimeWarp = root["render_manager_parameters"]["time_warp"]["max_ms_before_vsync"].asFloat();
 
  m_renderOverfillFactor = root["render_manager_parameters"]["render_overfill_factor"].asFloat();
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

unsigned RenderManagerConfig::getDisplayIndex() const
{
  return m_displayIndex;
}

bool RenderManagerConfig::getDirectHighPriority() const
{
  return m_directHighPriority;
}

unsigned RenderManagerConfig::getNumBuffers() const
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

int RenderManagerConfig::getWindowXPosition() const
{
  return m_windowXPosition;
}

int RenderManagerConfig::getWindowYPosition() const
{
  return m_windowYPosition;
}

unsigned RenderManagerConfig::getDisplayRotation() const
{
  return m_displayRotation;
}

unsigned RenderManagerConfig::getBitsPerColor() const
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