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
#include <vector>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <thread>
#include <chrono>

namespace osvr {
namespace vbtracker {
    class FakeImageSource : public ImageSource {
      public:
        FakeImageSource(std::string const &imagesDir);
        virtual ~FakeImageSource() {}

        bool ok() const override { return !m_images.empty(); }
        bool grab() override;
        void retrieveColor(cv::Mat &color) override;
        cv::Size resolution() const override;

      private:
        std::vector<cv::Mat> m_images;
        size_t m_currentImage = 0;
        cv::Size m_res;
    };

    ImageSourcePtr openImageFileSequence(std::string const &dir) {
        auto ret = ImageSourcePtr{new FakeImageSource{dir}};
        if (!ret->ok()) {
            // if we couldn't load, reset the pointer right now.
            ret.reset();
        }
        return ret;
    }
    FakeImageSource::FakeImageSource(std::string const &imagesDir) {

        // Read a vector of images, which we'll loop through.
        for (int imageNum = 1;; ++imageNum) {
            std::ostringstream fileName;
            fileName << imagesDir << "/";
            fileName << std::setfill('0') << std::setw(4) << imageNum;
            fileName << ".tif";
            cv::Mat image;
            std::cout << "Trying to read image from " << fileName.str()
                      << std::endl;
            image = cv::imread(fileName.str(), CV_LOAD_IMAGE_COLOR);
            if (!image.data) {
                break;
            }
            m_images.push_back(image);
        }
    }
    bool FakeImageSource::grab() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        m_currentImage = (m_currentImage + 1) % m_images.size();
        return ok();
    }

    void FakeImageSource::retrieveColor(cv::Mat &color) {
        m_images[m_currentImage].copyTo(color);
    }

    cv::Size FakeImageSource::resolution() const {
        return m_images.front().size();
    }

} // namespace vbtracker
} // namespace osvr
