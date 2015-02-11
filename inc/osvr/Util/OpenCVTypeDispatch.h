/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_OpenCVTypeDispatch_h_GUID_BA2EFF7D_152D_4FA6_6454_B8F3AC30FB3F
#define INCLUDED_OpenCVTypeDispatch_h_GUID_BA2EFF7D_152D_4FA6_6454_B8F3AC30FB3F

// Internal Includes
#include <osvr/Util/StdInt.h>
#include <osvr/Util/NumberTypeManipulation.h>

// Library/third-party includes
#include <opencv2/core/types_c.h>
#include <boost/mpl/identity.hpp>

// Standard includes
// - none

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
} // namespace util
} // namespace osvr

#endif // INCLUDED_OpenCVTypeDispatch_h_GUID_BA2EFF7D_152D_4FA6_6454_B8F3AC30FB3F
