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
#include "Oculus_DK2.h"

// Library/third-party includes
#include <opencv2/imgproc/imgproc.hpp>

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    class DK2WrappedImageSource : public ImageSource {
      public:
        DK2WrappedImageSource(ImageSourcePtr &&cam, bool doHid)
            : m_camera(std::move(cam)) {
            if (doHid) {
                m_hid.reset(new oculus_dk2::Oculus_DK2_HID{});
            }
        }
        virtual ~DK2WrappedImageSource() {}

        bool ok() const override { return m_camera && m_camera->ok(); }
        bool grab() override;
        void retrieve(cv::Mat &color, cv::Mat &gray) override;
        cv::Size resolution() const override;
        void retrieveColor(cv::Mat &color) override;

      private:
        ImageSourcePtr m_camera;
        cv::Mat m_scratch;
        std::unique_ptr<oculus_dk2::Oculus_DK2_HID> m_hid;
    };

    ImageSourcePtr openDK2WrappedCamera(ImageSourcePtr &&cam, bool doHid) {
        auto ret = ImageSourcePtr{};
        if (!cam->ok()) {
            // ditch failed cams right away
            return ret;
        }
        ret.reset(new DK2WrappedImageSource(std::move(cam), doHid));
        return ret;
    }

    bool DK2WrappedImageSource::grab() {
        if (m_hid) {
            m_hid->poll();
        }
        return m_camera->grab();
    }

    void DK2WrappedImageSource::retrieve(cv::Mat &color, cv::Mat &gray) {

        m_camera->retrieveColor(m_scratch);

        gray = osvr::oculus_dk2::unscramble_image(m_scratch);
        cv::cvtColor(gray, color, CV_GRAY2RGB);
    }

    void DK2WrappedImageSource::retrieveColor(cv::Mat &color) {
        cv::Mat dummy;
        retrieve(color, dummy);
    }

    cv::Size DK2WrappedImageSource::resolution() const {
        return m_camera->resolution();
    }

} // namespace vbtracker
} // namespace osvr
