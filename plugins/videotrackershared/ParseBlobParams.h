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

#ifndef INCLUDED_ParseBlobParams_h_GUID_D159000A_2DAE_4C96_3DB1_C22A119C7513
#define INCLUDED_ParseBlobParams_h_GUID_D159000A_2DAE_4C96_3DB1_C22A119C7513

// Internal Includes
#include <BlobParams.h>
#include <EdgeHoleBasedLedExtractor.h>
#include <GetOptionalParameter.h>

// Library/third-party includes
#include <json/value.h>

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    inline void parseBlobParams(Json::Value const &blob, BlobParams &p) {
        getOptionalParameter(p.absoluteMinThreshold, blob,
                             "absoluteMinThreshold");
        getOptionalParameter(p.minDistBetweenBlobs, blob,
                             "minDistBetweenBlobs");
        getOptionalParameter(p.minArea, blob, "minArea");
        getOptionalParameter(p.filterByCircularity, blob,
                             "filterByCircularity");
        getOptionalParameter(p.minCircularity, blob, "minCircularity");
        getOptionalParameter(p.filterByConvexity, blob, "filterByConvexity");
        getOptionalParameter(p.minConvexity, blob, "minConvexity");
        getOptionalParameter(p.minThresholdAlpha, blob, "minThresholdAlpha");
        getOptionalParameter(p.maxThresholdAlpha, blob, "maxThresholdAlpha");
        getOptionalParameter(p.thresholdSteps, blob, "thresholdSteps");
    }

    inline void
    parseEdgeHoleExtractorParams(Json::Value const &config,
                                 EdgeHoleBasedLedExtractor::Params &p) {
        getOptionalParameter(p.preEdgeDetectionBlurSize, config,
                             "preEdgeDetectionBlurSize");
        getOptionalParameter(p.laplacianKSize, config, "laplacianKSize");
        getOptionalParameter(p.laplacianScale, config, "laplacianScale");
        getOptionalParameter(p.postEdgeDetectionBlur, config,
                             "postEdgeDetectionBlur");
        getOptionalParameter(p.postEdgeDetectionBlurSize, config,
                             "postEdgeDetectionBlurSize");
        getOptionalParameter(p.postEdgeDetectionBlurThreshold, config,
                             "postEdgeDetectionBlurThreshold");
    }
} // End namespace vbtracker
} // End namespace osvr

#endif // INCLUDED_ParseBlobParams_h_GUID_D159000A_2DAE_4C96_3DB1_C22A119C7513
