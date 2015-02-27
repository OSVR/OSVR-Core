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

#ifndef INCLUDED_Imaging_h_GUID_5473F500_E901_419D_46D2_62ED5CB57412
#define INCLUDED_Imaging_h_GUID_5473F500_E901_419D_46D2_62ED5CB57412

// Internal Includes
#include <osvr/Util/ImagingReportTypesC.h>

// Library/third-party includes
#include <boost/shared_ptr.hpp>

// Standard includes
// - none

namespace osvr {

namespace clientkit {
    typedef boost::shared_ptr<OSVR_ImageBufferElement> ImageBufferPtr;
    /// @todo method to wrap a buffer in the pointer with the right deleter
    /// @todo class for the interface so that the deleter can be bound to the
    /// context?
    /// @todo wrap callback to perform cv::Mat header creation

} // end namespace clientkit

} // end namespace osvr

#endif // INCLUDED_Imaging_h_GUID_5473F500_E901_419D_46D2_62ED5CB57412
