/** @file
    @brief Header

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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_OpenCVTypeDispatch_h_GUID_BA2EFF7D_152D_4FA6_6454_B8F3AC30FB3F
#define INCLUDED_OpenCVTypeDispatch_h_GUID_BA2EFF7D_152D_4FA6_6454_B8F3AC30FB3F

// Internal Includes
#include <osvr/Util/StdInt.h>
#include <osvr/Util/NumberTypeManipulation.h>
#include <osvr/Util/ImagingReportTypesC.h>

// Library/third-party includes
#include <opencv2/core/version.hpp>
#if CV_MAJOR_VERSION == 2 || CV_VERSION_EPOCH == 2
#include <opencv2/core/core_c.h>
#else
#include <opencv2/core/core.hpp>
#endif
#include <boost/mpl/identity.hpp>

// Standard includes
#include <stdexcept>

namespace osvr {
namespace util {
    /// @brief Given an OpenCV type depth and a reference to a functor, calls
    /// that functor with a single parameter: the corresponding type wrapped in
    /// boost::mpl::identity.
    ///
    /// @returns false if we couldn't recognize the OpenCV type and thus didn't
    /// call your functor.
    template <typename Functor>
    inline bool opencvTypeDispatch(int openCVType, Functor &f) {
        using boost::mpl::identity;
        switch (CV_MAT_DEPTH(openCVType)) {
        case CV_8U:
            f(identity<uint8_t>());
            break;
        case CV_8S:
            f(identity<int8_t>());
            break;
        case CV_16U:
            f(identity<uint16_t>());
            break;
        case CV_16S:
            f(identity<int16_t>());
            break;
        case CV_32S:
            f(identity<int32_t>());
            break;
        case CV_32F:
            f(identity<float>());
            break;
        case CV_64F:
            f(identity<double>());
            break;
        default:
            return false;
        };
        return true;
    }

    inline NumberTypeData opencvNumberTypeData(int openCVType) {
        NumberTypeData ret;
        detail::NumberTypeDecompositionFunctor f(ret);
        opencvTypeDispatch(openCVType, f);
        return ret;
    }

    /// @brief Computes the OpenCV "type" (also known as depth - as in CV_8U)
    /// for a given combination of signed, float, and byte depth.
    inline int cvTypeFromData(bool isSigned, bool isFloat, size_t depth) {
        switch (depth) {
        case 1:
            return isSigned ? CV_8S : CV_8U;
        case 2:
            return isSigned ? CV_16S : CV_16U;

        case 4:
            if (isFloat) {
                return CV_32F;
            }
            if (isSigned) {
                return CV_32S;
            }
            throw std::runtime_error("No OpenCV 32-bit unsigned type!");
        case 8:
            if (isFloat) {
                return CV_64F;
            }
            throw std::runtime_error("No OpenCV 64-bit integer types!");
        default:

            throw std::runtime_error(
                "No OpenCV type matching the requested parameters!");
        }
    }

    /// @brief Computes the OpenCV matrix type (as in CV_8UC3) from a metadata
    /// struct
    inline int computeOpenCVMatType(OSVR_ImagingMetadata const &metadata) {
        return CV_MAKETYPE(
            cvTypeFromData(metadata.type == OSVR_IVT_SIGNED_INT,
                           metadata.type == OSVR_IVT_FLOATING_POINT,
                           metadata.depth),
            metadata.channels);
    }
} // namespace util
} // namespace osvr

#endif // INCLUDED_OpenCVTypeDispatch_h_GUID_BA2EFF7D_152D_4FA6_6454_B8F3AC30FB3F
