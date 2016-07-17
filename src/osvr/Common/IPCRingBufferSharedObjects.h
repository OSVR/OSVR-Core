/** @file
    @brief Header defining the types placed into shared memory for an
   IPCRingBuffer

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

#ifndef INCLUDED_IPCRingBufferSharedObjects_h_GUID_890394B4_253B_4BAF_C813_E0E54D159128
#define INCLUDED_IPCRingBufferSharedObjects_h_GUID_890394B4_253B_4BAF_C813_E0E54D159128

// Internal Includes
#include "IPCRingBufferResults.h"
#include "SharedMemoryObjectWithMutex.h"
#include <osvr/Common/IPCRingBuffer.h>
#include <osvr/Util/StdInt.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <utility>

namespace osvr {
namespace common {

    namespace detail {
        namespace bip = boost::interprocess;

        class ElementData : public ipc::ObjectWithMutex, boost::noncopyable {
          public:
            typedef IPCRingBuffer::value_type BufferType;

            ElementData() : m_buf(nullptr) {}

            template <typename LockType>
            BufferType *getBuf(LockType &lock) const {
                verifyReaderLock(lock);
                return m_buf.get();
            }

            template <typename ManagedMemory>
            void allocateBuf(ManagedMemory &shm,
                             IPCRingBuffer::Options const &opts) {
                freeBuf(shm);
                m_buf = static_cast<BufferType *>(shm.allocate_aligned(
                    opts.getEntrySize(), opts.getAlignment()));
            }

            template <typename ManagedMemory> void freeBuf(ManagedMemory &shm) {
                if (nullptr != m_buf) {
                    shm.deallocate(m_buf.get());
                }
                m_buf = nullptr;
            }

          private:
            ipc_offset_ptr<BufferType> m_buf;
        };

        class Bookkeeping : public ipc::ObjectWithMutex, boost::noncopyable {
          public:
            typedef IPCRingBuffer::sequence_type sequence_type;
            typedef uint16_t raw_index_type;

            template <typename ManagedMemory>
            static Bookkeeping *find(ManagedMemory &shm) {
                auto self =
                    shm.template find<Bookkeeping>(bip::unique_instance);
                return self.first;
            }

            template <typename ManagedMemory>
            static Bookkeeping *construct(ManagedMemory &shm,
                                          IPCRingBuffer::Options const &opts) {
                return shm.template construct<Bookkeeping>(
                    bip::unique_instance)(shm, opts);
            }

            template <typename ManagedMemory>
            static void destroy(ManagedMemory &shm) {
                auto self = find(shm);
                if (nullptr == self) {
                    return;
                }
                self->freeBufs(shm);
                shm.template destroy<Bookkeeping>(bip::unique_instance);
            }

            template <typename ManagedMemory>
            Bookkeeping(ManagedMemory &shm, IPCRingBuffer::Options const &opts)
                : m_capacity(opts.getEntries()),
                  elementArray(shm.template construct<ElementData>(
                      bip::unique_instance)[m_capacity]()),
                  m_beginSequenceNumber(0), m_nextSequenceNumber(0), m_begin(0),
                  m_size(0), m_bufLen(opts.getEntrySize()) {

                auto lock = getExclusiveLock();
                {
                    for (raw_index_type i = 0; i < m_capacity; ++i) {
                        try {
                            getByRawIndex(i, lock).allocateBuf(shm, opts);
                        } catch (std::bad_alloc &) {
                            OSVR_DEV_VERBOSE("Couldn't allocate buffer #"
                                             << i
                                             << ", truncating the ring buffer");
                            m_capacity = i;
                            break;
                        }
                    }
                }
            }

            template <typename ManagedMemory>
            void freeBufs(ManagedMemory &shm) {
                auto lock = getExclusiveLock();
                {
                    for (raw_index_type i = 0; i < m_capacity; ++i) {
                        getByRawIndex(i, lock).freeBuf(shm);
                    }
                    shm.template destroy<ElementData>(bip::unique_instance);
                }
            }

            /// @brief Get number of elements.
            raw_index_type getCapacity() const { return m_capacity; }

            /// @brief Get capacity of elements.
            uint32_t getBufferLength() const { return m_bufLen; }

            template <typename LockType>
            ElementData &getByRawIndex(raw_index_type index, LockType &lock) {
                verifyReaderLock(lock);
                return *(elementArray + (index % m_capacity));
            }
            template <typename LockType>
            ElementData *getBySequenceNumber(sequence_type num,
                                             LockType &lock) {
                verifyReaderLock(lock);
                auto sequenceRelativeToBegin = num - m_beginSequenceNumber;
                if (sequenceRelativeToBegin < m_size) {
                    raw_index_type idx((m_begin + sequenceRelativeToBegin) %
                                       m_capacity);
                    return &getByRawIndex(idx, lock);
                }
                return nullptr; // out of bounds request -> nullptr return.
            }

            template <typename LockType> bool empty(LockType &lock) const {
                verifyReaderLock(lock);
                return m_size == 0;
            }
            template <typename LockType>
            sequence_type backSequenceNumber(LockType &lock) {
                verifyReaderLock(lock);
                return m_nextSequenceNumber - 1;
            }

            template <typename LockType> ElementData *back(LockType &lock) {
                verifyReaderLock(lock);
                if (empty(lock)) {
                    return nullptr;
                }
                return &getByRawIndex(m_begin + m_size - 1, lock);
            }

            IPCPutResultPtr produceElement() {
                auto lock = getExclusiveLock();
                auto sequenceNumber = m_nextSequenceNumber;
                m_nextSequenceNumber++;
                if (m_size == m_capacity) {
                    m_begin++;
                    m_beginSequenceNumber++;
                } else {
                    m_size++;
                }
#ifdef OSVR_SHM_LOCK_DEBUGGING
                OSVR_DEV_VERBOSE(
                    "Attempting to get an exclusive lock on sequence "
                    << sequenceNumber << " aka index " << back(lock));
#endif
                auto elementLock = back(lock)->getExclusiveLock();
                /// shared memory nullptr filled in by outer class
                IPCPutResultPtr ret(new IPCPutResult{
                    back(lock)->getBuf(elementLock), sequenceNumber,
                    std::move(elementLock), std::move(lock), nullptr});
                return ret;
            }

          private:
            raw_index_type m_capacity;
            ipc_offset_ptr<ElementData> elementArray;
            IPCRingBuffer::sequence_type m_beginSequenceNumber;
            IPCRingBuffer::sequence_type m_nextSequenceNumber;
            raw_index_type m_begin;
            raw_index_type m_size;
            uint32_t m_bufLen;
        };
    } // namespace detail

} // namespace common
} // namespace osvr
#endif // INCLUDED_IPCRingBufferSharedObjects_h_GUID_890394B4_253B_4BAF_C813_E0E54D159128
