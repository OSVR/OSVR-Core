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

#ifndef INCLUDED_Imaging_decl_h_GUID_FB2A618F_DFF2_4CA1_B56E_C849105D480E
#define INCLUDED_Imaging_decl_h_GUID_FB2A618F_DFF2_4CA1_B56E_C849105D480E

// Internal Includes
#include <osvr/Util/ImagingReportTypesC.h>
#include <osvr/Util/TimeValue_fwd.h>
#include <osvr/Util/Deletable.h>

// Library/third-party includes
#include <boost/shared_ptr.hpp>
#include <opencv2/core/core.hpp>

// Standard includes
// - none

namespace osvr {

namespace clientkit {
    /// @brief A smart pointer controlling deletion of the image buffer. The
    /// buffer is automatically wrapped in one of these before your callback
    /// gets it.
    typedef boost::shared_ptr<OSVR_ImageBufferElement> ImageBufferPtr;

    /// @brief The user-friendly imaging report. Note that passing this around
    /// by value is OK (doesn't copy the image), and the easiest way to ensure
    /// that the image buffer doesn't get freed before you're done using it.
    struct ImagingReportOpenCV {
        /// @brief The device sensor number this frame came from.
        OSVR_ChannelCount sensor;

        /// @brief An OpenCV Mat header for accessing the image data in the
        /// buffer. Does not store the data internally (to reduce copies)!
        cv::Mat frame;

        /// @brief A shared pointer with custom deleter that owns the underlying
        /// image data buffer for the frame.
        ImageBufferPtr buffer;
    };

    /// @brief The user-friendly imaging callback type
    typedef void (*ImagingCallbackOpenCV)(
        void *userdata, util::time::TimeValue const &timestamp,
        ImagingReportOpenCV report);

    /// @brief Register a callback to receive each new full frame of imaging
    /// data.
} // end namespace clientkit

} // end namespace osvr

#endif // INCLUDED_Imaging_decl_h_GUID_FB2A618F_DFF2_4CA1_B56E_C849105D480E
