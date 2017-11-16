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
#include "CheckFirmwareVersion.h"
#include "DirectShowHDKCameraFactory.h"
#include "DirectShowToCV.h"
#include "ImageSourceFactories.h"

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

namespace osvr {
namespace vbtracker {
    using directx_camera_server_ptr = std::unique_ptr<directx_camera_server>;
    class DirectShowImageSource : public ImageSource {
      public:
        DirectShowImageSource(directx_camera_server_ptr &&cam)
            : m_camera(std::move(cam)) {
            m_camera->read_image_to_memory();
            if (m_camera->isOpened()) {
                storeRes();
            }
        }
        virtual ~DirectShowImageSource() {}

        bool ok() const override {
            return m_camera && m_gotRes && m_camera->working() &&
                   m_camera->isOpened();
        }
        bool grab() override;
        void retrieveColor(cv::Mat &color,
                           osvr::util::time::TimeValue &timestamp) override;
        cv::Size resolution() const override;

      private:
        void storeRes();
        directx_camera_server_ptr m_camera;
        bool m_gotRes = false;
        cv::Size m_res;
    };

    ImageSourcePtr openHDKCameraDirectShow(bool highGain) {
        auto ret = ImageSourcePtr{};
        auto cam = getDirectShowHDKCamera(highGain);
        if (!cam) {
            return ret;
        }

        // Check firmware version
        {
            static const auto UPDATER_URL = "osvr.github.io";
            auto firmwareStatus = checkCameraFirmwareRevision(cam->getPath());
            switch (firmwareStatus) {
            case osvr::vbtracker::FirmwareStatus::Good:
                break;
            case FirmwareStatus::Future:
                std::cerr
                    << "\n[Video-based Tracking] Note: Camera firmware "
                       "version detected was newer than recognized latest "
                       "version, assuming OK. You may want to update your "
                       "OSVR server or plugin!\n"
                    << std::endl;
                break;
            case osvr::vbtracker::FirmwareStatus::UpgradeRequired:
            case osvr::vbtracker::FirmwareStatus::UpgradeUseful:
                std::cerr
                    << "\n[Video-based Tracking] WARNING - Your HDK infrared "
                       "tracking camera was detected to have outdated "
                       "firmware in need of updating, and may not function "
                       "properly. Please visit "
                    << UPDATER_URL << " to get the "
                                      "camera firmware updater.\n\n"
                    << std::endl;
                /// @todo do we allow the tracker to run with outdated firmware
                /// versions?
                break;
            case osvr::vbtracker::FirmwareStatus::Unknown:
                std::cerr
                    << "\n[Video-based Tracking] Note: Could not detect the "
                       "firmware version on your HDK infrared "
                       "tracking camera. You may need to update it: see "
                    << UPDATER_URL << " to get the camera firmware updater.\n\n"
                    << std::endl;
                break;
            default:
                break;
            }

            if (firmwareStatus ==
                osvr::vbtracker::FirmwareStatus::UpgradeRequired) {
                /// this enum value is reserved for versions so old that they
                /// don't sync correctly with the LED pulses...
                std::cerr << "\n[Video-based Tracking] The video tracking "
                             "plugin cannot run with your camera until the "
                             "firmware has been updated.\n\n"
                          << std::endl;
                return ret;
            }
        }
        ret.reset(new DirectShowImageSource{std::move(cam)});
        return ret;
    }

    bool DirectShowImageSource::grab() {
        return m_camera->read_image_to_memory();
    }
    void DirectShowImageSource::retrieveColor(
        cv::Mat &color, osvr::util::time::TimeValue &timestamp) {
        color = ::retrieve(*m_camera);
        timestamp = m_camera->get_buffer_timestamp();
    }
    cv::Size DirectShowImageSource::resolution() const { return m_res; }

    void DirectShowImageSource::storeRes() {
        int minx, miny, maxx, maxy;
        m_camera->read_range(minx, maxx, miny, maxy);
        m_res = cv::Size(maxx - minx + 1, maxy - miny + 1);
        m_gotRes = true;
    }
} // namespace vbtracker
} // namespace osvr
