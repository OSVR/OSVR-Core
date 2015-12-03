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
#include "ImageSourceFactories.h"
#include "DirectShowHDKCameraFactory.h"
#include "DirectShowToCV.h"

// Library/third-party includes
// - none

// Standard includes
// - none

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
        void retrieveColor(cv::Mat &color) override;
        cv::Size resolution() const override;

      private:
        void storeRes();
        directx_camera_server_ptr m_camera;
        bool m_gotRes = false;
        cv::Size m_res;
    };
    ImageSourcePtr openHDKCameraDirectShow() {

        auto ret = ImageSourcePtr{};
        auto cam = getDirectShowHDKCamera();
        if (!cam) {
            return ret;
        }
        ret.reset(new DirectShowImageSource{std::move(cam)});
        return ret;
    }
    bool DirectShowImageSource::grab() {
        return m_camera->read_image_to_memory();
    }
    void DirectShowImageSource::retrieveColor(cv::Mat &color) {
        color = ::retrieve(*m_camera);
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
