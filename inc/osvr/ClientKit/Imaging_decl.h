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

#ifndef INCLUDED_Imaging_decl_h_GUID_FB2A618F_DFF2_4CA1_B56E_C849105D480E
#define INCLUDED_Imaging_decl_h_GUID_FB2A618F_DFF2_4CA1_B56E_C849105D480E

// Internal Includes
#include <osvr/Util/ImagingReportTypesC.h>
#include <osvr/Util/TimeValue_fwd.h>
#include <osvr/Util/Deletable.h>

// Library/third-party includes
#include <boost/shared_ptr.hpp>

// Standard includes
// - none

namespace osvr {

namespace clientkit {
    /// @addtogroup ClientKitCPP
    /// @{
    /// @brief A smart pointer controlling deletion of the image buffer. The
    /// buffer is automatically wrapped in one of these before your callback
    /// gets it.
    typedef boost::shared_ptr<OSVR_ImageBufferElement> ImageBufferPtr;

    /// @brief The user-friendly imaging report. Note that passing this around
    /// by value is OK (doesn't copy the image), and the easiest way to ensure
    /// that the image buffer doesn't get freed before you're done using it.
    struct ImagingReport {
        /// @brief The device sensor number this frame came from.
        OSVR_ChannelCount sensor;

        /// @brief Metadata containing the properties of this frame.
        OSVR_ImagingMetadata metadata;

        /// @brief A shared pointer with custom deleter that owns the underlying
        /// image data buffer for the frame.
        ImageBufferPtr buffer;
    };

    /// @brief The user-friendly imaging callback type
    typedef void (*ImagingCallback)(
        void *userdata, util::time::TimeValue const &timestamp,
        ImagingReport report);

    /// @}
} // end namespace clientkit

} // end namespace osvr

#endif // INCLUDED_Imaging_decl_h_GUID_FB2A618F_DFF2_4CA1_B56E_C849105D480E
