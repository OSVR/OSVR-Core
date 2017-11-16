/** @file
    @brief Header

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
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

///////
// Portions based on modules/imgproc/src/deriv.cpp from the OpenCV 2.4 branch,
// with this license header:
///////
// clang-format off

//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                        Intel License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of Intel Corporation may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.

// clang-format on

#ifndef INCLUDED_RealtimeLaplacian_h_GUID_1ECD4EFB_BF33_479D_0ED7_890BF096056E
#define INCLUDED_RealtimeLaplacian_h_GUID_1ECD4EFB_BF33_479D_0ED7_890BF096056E

// Internal Includes
// - none

// Library/third-party includes
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Standard includes
// - none

namespace osvr {
namespace vbtracker {

    /// A class designed for real-time use of a Laplacian on similar images over
    /// and over, based on the OpenCV implementation but avoiding duplicate work
    /// and using persistent temporary storage when possible.
    class RealtimeLaplacian {
      public:
        RealtimeLaplacian(int destDepth, int kSize = 3, double scale = 1.,
                          double delta = 0.,
                          int borderType = cv::BORDER_DEFAULT)
            : kSize_(kSize), destDepth_(destDepth), scale_(scale),
              delta_(delta), borderType_(borderType) {}

        void apply(cv::InputArray mySrc, cv::OutputArray myDst) {
            if (kSize_ == 1 || kSize_ == 3) {
                // small enough to just use the stock stuff, which doesn't
                // allocate things
                cv::Laplacian(mySrc, myDst, destDepth_, kSize_, scale_, delta_,
                              borderType_);
                return;
            }
            cv::Mat src = mySrc.getMat();
            if (setup_) {
                /// check to make sure nothing changed, or we set it all up
                /// again.
                setup_ = doesImageMatchCache(src);
            }
            if (!setup_) {
                /// Cache the data from the source image.
                /// Must be kept in sync with doesImageMatchCache()
                srcDepth_ = src.depth();
                srcChannels_ = src.channels();
                srcCols_ = src.cols;
                srcRows_ = src.rows;
                srcType_ = src.type();
                /// Actually go produce the things we need.
                setupImpl();
            }

            /// ideally this is already allocated...
            myDst.create(src.size(), CV_MAKETYPE(destDepth_, src.channels()));

            cv::Mat dst = myDst.getMat();

            /// the remaining body of cv::Laplacian that we couldn't cache
            /// between calls.
            int y = fx_->start(src), dsty = 0, dy = 0;
            fy_->start(src);
            const uchar *sptr = src.data + y * src.step;

            for (; dsty < src.rows; sptr += dy0_ * src.step, dsty += dy) {
                fx_->proceed(sptr, (int)src.step, dy0_, d2x_.data,
                             (int)d2x_.step);
                dy = fy_->proceed(sptr, (int)src.step, dy0_, d2y_.data,
                                  (int)d2y_.step);
                if (dy > 0) {
                    /// this is just creating a new header, not allocating new
                    /// memory.
                    cv::Mat dstripe = dst.rowRange(dsty, dsty + dy);
                    d2x_.rows = d2y_.rows =
                        dy; // modify the headers, which should work
                    d2x_ += d2y_;
                    d2x_.convertTo(dstripe, destType_, scale_, delta_);
                }
            }
        }

      private:
        bool doesImageMatchCache(cv::Mat const &src) const {
            /// must be kept in sync with the if (!setup_) section in apply()
            return (srcDepth_ == src.depth()) &&
                   (srcChannels_ == src.channels()) && (srcCols_ == src.cols) &&
                   (srcRows_ == src.rows) && (srcType_ == src.type());
        }
        int ktype() const {
            return std::max<int>({CV_32F, destDepth_, srcDepth_});
        }
        void setupImpl() {

            int ktype = std::max(CV_32F, std::max(destDepth_, srcDepth_));
            int wdepth = srcDepth_ == CV_8U && kSize_ <= 5
                             ? CV_16S
                             : srcDepth_ <= CV_32F ? CV_32F : CV_64F;
            workType_ = CV_MAKETYPE(wdepth, srcChannels_);

            // internally calls getSobelKernels
            cv::getDerivKernels(kd_, ks_, 2, 0, kSize_, false, ktype);

            destType_ = CV_MAKETYPE(destDepth_, srcChannels_);
            dy0_ =
                std::min(std::max((int)(STRIPE_SIZE /
                                        (cv::getElemSize(srcType_) * srcCols_)),
                                  1),
                         srcRows_);
            fx_ = cv::createSeparableLinearFilter(
                srcType_, workType_, kd_, ks_, cv::Point(-1, -1), 0,
                borderType_, borderType_, cv::Scalar());
            fy_ = cv::createSeparableLinearFilter(
                srcType_, workType_, ks_, kd_, cv::Point(-1, -1), 0,
                borderType_, borderType_, cv::Scalar());
            makeD2mats();
            setup_ = true;
        }
        void makeD2mats() {
            d2x_ = cv::Mat(dy0_ + kd_.rows - 1, srcCols_, workType_);
            d2y_ = cv::Mat(dy0_ + kd_.rows - 1, srcCols_, workType_);
        }

        /// @name One-time parameters from constructor
        /// @{
        const int kSize_;
        const int destDepth_;
        const double scale_;
        const double delta_;
        const int borderType_;
        /// @}

        /// Whether the cached from input fields are up to date and the
        /// generated by setupImpl fields match.
        bool setup_ = false;

        /// @name Cached from input
        /// @{
        int srcDepth_ = 0;
        int srcChannels_ = 0;
        int srcCols_ = 0;
        int srcRows_ = 0;
        int srcType_ = 0;
        /// @}
        /// @name Generated by setupImpl
        /// @{
        int workType_;
        int destType_;
        cv::Mat kd_;
        cv::Mat ks_;
        int dy0_;
        cv::Ptr<cv::FilterEngine> fx_;
        cv::Ptr<cv::FilterEngine> fy_;
        cv::Mat d2x_;
        cv::Mat d2y_;
        /// @}

        /// Constant from OpenCV
        static const size_t STRIPE_SIZE = 1 << 14;
    };

} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_RealtimeLaplacian_h_GUID_1ECD4EFB_BF33_479D_0ED7_890BF096056E
