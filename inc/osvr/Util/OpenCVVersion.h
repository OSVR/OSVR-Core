/** @file
    @brief Header defining a few simple preprocessor macros for dealing with
   OpenCV 2 and 3 compatibility.

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

#ifndef INCLUDED_OpenCVVersion_h_GUID_503D3091_E553_4AA0_435D_7CCAF2791E32
#define INCLUDED_OpenCVVersion_h_GUID_503D3091_E553_4AA0_435D_7CCAF2791E32

// Internal Includes
// - none

// Library/third-party includes
#include <opencv2/core/core.hpp>

// Standard includes
// - none

// Check for OpenCV 2 - using the "EPOCH" defines - to avoid getting confused
// later.
#if defined(CV_VERSION_EPOCH) && CV_VERSION_EPOCH == 2
#define OSVR_OPENCV_VER 2
#define OSVR_OPENCV_2
#endif

// Check for OpenCV 3.x
#if !defined(OSVR_OPENCV_VER) && defined(CV_MAJOR_VERSION) &&                  \
    CV_MAJOR_VERSION == 3
#define OSVR_OPENCV_VER 3
#define OSVR_OPENCV_3
#define OSVR_OPENCV_3PLUS
#endif

// Check for future versions
#if !defined(OSVR_OPENCV_VER) && defined(CV_MAJOR_VERSION) &&                  \
    CV_MAJOR_VERSION > 3
#define OSVR_OPENCV_VER CV_MAJOR_VERSION
#define OSVR_OPENCV_POST3
#define OSVR_OPENCV_3PLUS
#endif

// Check for OpenCV 2 that didn't get caught by the CV_VERSION_EPOCH check.
#if !defined(OSVR_OPENCV_VER) && defined(CV_MAJOR_VERSION) &&                  \
    CV_MAJOR_VERSION == 2
#define OSVR_OPENCV_VER 2
#define OSVR_OPENCV_2
#endif

// If, for some reason, CV_MAJOR_VERSION isn't defined, nor the
// CV_VERSION_EPOCH, we can assume at least for now that we have
// version 2.x
#if !defined(OSVR_OPENCV_VER)
#define OSVR_OPENCV_VER 2
#define OSVR_OPENCV_2
#define OSVR_OPENCV_VER_GUESSED
#endif

#ifdef OSVR_OPENCV_2

// Second component of the version number
#if defined(CV_MINOR_VERSION)
#define OSVR_OPENCV_VER_COMPONENT_2 CV_MINOR_VERSION
#elif defined(CV_VERSION_MAJOR)
#define OSVR_OPENCV_VER_COMPONENT_2 CV_VERSION_MAJOR
#else
// assume 0 if unknown
#define OSVR_OPENCV_VER_COMPONENT_2 0
#endif

// Third component of the version number
#if defined(CV_SUBMINOR_VERSION)
#define OSVR_OPENCV_VER_COMPONENT_3 CV_SUBMINOR_VERSION
#elif defined(CV_VERSION_MINOR)
#define OSVR_OPENCV_VER_COMPONENT_3 CV_VERSION_MINOR
#else
// assume 0 if unknown
#define OSVR_OPENCV_VER_COMPONENT_2 0
#endif

#else // OpenCV 3 and up - easier case

#define OSVR_OPENCV_VER_COMPONENT_2 CV_MINOR_VERSION

#ifdef CV_VERSION_REVISION
#define OSVR_OPENCV_VER_COMPONENT_3 CV_VERSION_REVISION
#else
#define OSVR_OPENCV_VER_COMPONENT_3 CV_SUBMINOR_VERSION
#endif

#endif

/// Version checking macro: true at preprocessor time if OpenCV is at least
/// version X.Y.Z
#define OSVR_OPENCV_AT_LEAST_VERSION(X, Y, Z)                                  \
    ((OSVR_OPENCV_VER > X) ||                                                  \
     (OSVR_OPENCV_VER == X && OSVR_OPENCV_VER_COMPONENT_2 > Y) ||              \
     (OSVR_OPENCV_VER == X && OSVR_OPENCV_VER_COMPONENT_2 == Y &&              \
      OSVR_OPENCV_VER_COMPONENT_3 >= Z))

#endif // INCLUDED_OpenCVVersion_h_GUID_503D3091_E553_4AA0_435D_7CCAF2791E32
