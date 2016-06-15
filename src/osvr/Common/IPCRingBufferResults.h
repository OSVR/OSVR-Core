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
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_IPCRingBufferResults_h_GUID_324D713B_59B5_478A_4FA4_81851E72E83E
#define INCLUDED_IPCRingBufferResults_h_GUID_324D713B_59B5_478A_4FA4_81851E72E83E

// Internal Includes
#include "SharedMemory.h"
#include "SharedMemoryObjectWithMutex.h"
#include <osvr/Common/IPCRingBuffer.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {

    namespace detail {
        struct IPCPutResult {
            ~IPCPutResult() {
#ifdef OSVR_SHM_LOCK_DEBUGGING
                OSVR_DEV_VERBOSE("Releasing exclusive lock on sequence "
                                 << seq);
#endif
                elementLock.unlock();
                boundsLock.unlock();
            }
            IPCRingBuffer::value_type *buffer;
            IPCRingBuffer::sequence_type seq;
            ipc::exclusive_lock_type elementLock;
            ipc::exclusive_lock_type boundsLock;
            IPCRingBufferPtr shm;
        };

        struct IPCGetResult {

            ~IPCGetResult() {
#ifdef OSVR_SHM_LOCK_DEBUGGING
                OSVR_DEV_VERBOSE("Releasing shared lock on sequence " << seq);
#endif
                elementLock.unlock();
            }
            IPCRingBuffer::value_type *buffer;
            ipc::sharable_lock_type elementLock;
            IPCRingBuffer::sequence_type seq;
            IPCRingBufferPtr shm;
        };
    } // namespace detail

} // namespace common
} // namespace osvr
#endif // INCLUDED_IPCRingBufferResults_h_GUID_324D713B_59B5_478A_4FA4_81851E72E83E
