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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_AlignedMemoryUniquePtr_h_GUID_E68E5347_3A15_4289_4D16_1C9E7B1BCF45
#define INCLUDED_AlignedMemoryUniquePtr_h_GUID_E68E5347_3A15_4289_4D16_1C9E7B1BCF45

// Internal Includes
#include <osvr/Util/AlignedMemory.h>
#include <osvr/Util/UniquePtr.h>
#include <osvr/Util/ImagingReportTypesC.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace util {
    using AlignedPtr = unique_ptr<void, AlignedAllocDeleter>;
    using AlignedImageBufferPtr =
        unique_ptr<OSVR_ImageBufferElement, AlignedAllocDeleter>;

    inline AlignedPtr
    makeAlignedBuffer(size_t bytes,
                      size_t alignment = OSVR_DEFAULT_ALIGN_SIZE) {
        AlignedPtr ret(util::alignedAlloc(bytes, alignment));
        return ret;
    }
    inline AlignedImageBufferPtr
    makeAlignedImageBuffer(size_t bytes,
                           size_t alignment = OSVR_DEFAULT_ALIGN_SIZE) {
        AlignedImageBufferPtr ret(reinterpret_cast<OSVR_ImageBufferElement *>(
            util::alignedAlloc(bytes, alignment)));
        return ret;
    }
} // namespace util
} // namespace osvr

#endif // INCLUDED_AlignedMemoryUniquePtr_h_GUID_E68E5347_3A15_4289_4D16_1C9E7B1BCF45
