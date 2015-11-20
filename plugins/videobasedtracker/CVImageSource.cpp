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

// Library/third-party includes
#include <opencv2/highgui/highgui.hpp> // for image capture

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    using CVCapturePtr = std::unique_ptr<cv::VideoCapture>;

    class OpenCVImageSource : public ImageSource {
      public:
        OpenCVImageSource(CVCapturePtr &&cam) : m_camera(std::move(cam)) {
            if (m_camera->isOpened()) {
                storeRes();
            }
        }
        virtual ~OpenCVImageSource() {}

        bool ok() const override { return m_camera && m_camera->isOpened(); }
        bool grab() override;
        void retrieveColor(cv::Mat &color) override;
        cv::Size resolution() const override;

      private:
        void storeRes();
        CVCapturePtr m_camera;
        cv::Size m_res;
    };

    ImageSourcePtr openOpenCVCamera(int which) {
        auto ret = ImageSourcePtr{};
        auto cam = CVCapturePtr{new cv::VideoCapture(which)};
        if (!cam->isOpened()) {
            // couldn't open the desired camera
            return ret;
        }
        ret.reset(new OpenCVImageSource{std::move(cam)});
        return ret;
    }

    bool OpenCVImageSource::grab() { return m_camera->grab(); }

    void OpenCVImageSource::retrieveColor(cv::Mat &color) {
        m_camera->retrieve(color);
    }

    cv::Size OpenCVImageSource::resolution() const { return m_res; }

    void OpenCVImageSource::storeRes() {
        int height = static_cast<int>(m_camera->get(CV_CAP_PROP_FRAME_HEIGHT));
        int width = static_cast<int>(m_camera->get(CV_CAP_PROP_FRAME_WIDTH));
        m_res = cv::Size(width, height);
    }
} // namespace vbtracker
} // namespace osvr
