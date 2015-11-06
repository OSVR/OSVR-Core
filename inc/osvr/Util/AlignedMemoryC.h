/** @file
    @brief Header

    Must be c-safe!

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

/*
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
*/

#ifndef INCLUDED_AlignedMemoryC_h_GUID_A6BA9A9A_EF49_4FA7_0312_635E151919E7
#define INCLUDED_AlignedMemoryC_h_GUID_A6BA9A9A_EF49_4FA7_0312_635E151919E7

/* Internal Includes */
#include <osvr/Util/Export.h>
#include <osvr/Util/APIBaseC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
#include <stdlib.h>

OSVR_EXTERN_C_BEGIN

enum {
    /** @brief The default (and core-utilized) alignment of imaging buffers,
        etc. */
    OSVR_DEFAULT_ALIGN_SIZE = 16
};

/** @brief Aligned allocation function, gives a pointer to a block of
    memory aligned to a memory boundary.
*/
OSVR_UTIL_EXPORT void *
osvrAlignedAlloc(size_t bytes,
                 size_t alignment OSVR_CPP_ONLY(= OSVR_DEFAULT_ALIGN_SIZE));
/** @brief Aligned deallocation function, uses the pointer to the original
    memory block to deallocate it.
*/
OSVR_UTIL_EXPORT void osvrAlignedFree(void *p);

OSVR_EXTERN_C_END

#endif
