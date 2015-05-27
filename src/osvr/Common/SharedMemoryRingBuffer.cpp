/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/Common/SharedMemoryRingBuffer.h>
#include "SharedMemory.h"
#include "SharedMemoryObjectWithMutex.h"
#include <osvr/Util/ImagingReportTypesC.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <boost/version.hpp>

// Standard includes
#include <stdexcept>
#include <utility>
#include <type_traits>

namespace osvr {
namespace common {
#define OSVR_SHM_VERBOSE(X) OSVR_DEV_VERBOSE("SharedMemoryRingBuffer: " << X)

    /// @brief the ABI level: this must be bumped if the layout of any shared-memory
    /// objects (Bookkeeping, ElementData) changes, if Boost Interprocess changes
    /// affect the utilized ABI, or if other changes occur that would interfere with
    /// communication.
    static SharedMemoryRingBuffer::abi_level_type SHM_SOURCE_ABI_LEVEL = 0;

    /// Some tests that can be automated for ensuring validity of the ABI level
    /// number.
#if (BOOST_VERSION > 105800)
#error                                                                         \
    "Using an untested Boost version - inspect the Boost Interprocess release notes/changelog to see if any ABI breaks affect us."
#endif

#ifdef _WIN32
#if (BOOST_VERSION < 105400)
#error                                                                         \
    "Boost Interprocess pre-1.54 on Win32 is ABI-incompatible with newer Boost due to changed bootstamp function."
#endif
#else // !_WIN32
    // No obvious ABI breaks through 1.58 seem to apply to us on non-Windows
    // platforms
#endif

    static_assert(std::is_same<SharedMemoryRingBuffer::BackendType,
                               ipc::SharedMemoryBackendType>::value,
                  "The typedefs SharedMemoryRingBuffer::BackendType and "
                  "ipc::SharedMemoryBackendType must remain in sync!");

    namespace bip = boost::interprocess;
    namespace {
        typedef OSVR_ImageBufferElement BufferType;

        template <typename T, typename ManagedMemory>
        using ipc_deleter_type =
            typename ManagedMemory::template deleter<T>::type;

        typedef SharedMemoryRingBuffer::sequence_type sequence_type;

        /// @brief Destroys the "unique_instance" of a given type in a managed
        /// memory segment. If there isn't an instance, this is a no-op.
        template <typename T, typename ManagedMemory>
        inline void destroy_unique_instance(ManagedMemory &shm) {
            auto result = shm.template find<T>(bip::unique_instance);
            if (result.first) {
                shm.template destroy<T>(bip::unique_instance);
            }
        }
    } // namespace
    namespace detail {
        struct IPCPutResult {
            ~IPCPutResult() {
#ifdef OSVR_SHM_LOCK_DEBUGGING
                OSVR_SHM_VERBOSE("Releasing exclusive lock on sequence "
                                 << seq);
#endif
                elementLock.unlock();
                boundsLock.unlock();
            }
            BufferType *buffer;
            sequence_type seq;
            ipc::exclusive_lock_type elementLock;
            ipc::exclusive_lock_type boundsLock;
            SharedMemoryRingBufferPtr shm;
        };

        struct IPCGetResult {

            ~IPCGetResult() {
#ifdef OSVR_SHM_LOCK_DEBUGGING
                OSVR_SHM_VERBOSE("Releasing shared lock on sequence " << seq);
#endif
                elementLock.unlock();
            }
            BufferType *buffer;
            ipc::sharable_lock_type elementLock;
            sequence_type seq;
            SharedMemoryRingBufferPtr shm;
        };
    } // namespace detail

    namespace {
        class ElementData : public ipc::ObjectWithMutex, boost::noncopyable {
          public:
            ElementData() : m_buf(nullptr) {}

            template <typename LockType>
            BufferType *getBuf(LockType &lock) const {
                verifyReaderLock(lock);
                return m_buf.get();
            }

            template <typename ManagedMemory>
            void allocateBuf(ManagedMemory &shm,
                             SharedMemoryRingBuffer::Options const &opts) {
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
            typedef uint16_t raw_index_type;
            template <typename ManagedMemory>
            Bookkeeping(ManagedMemory &shm,
                        SharedMemoryRingBuffer::Options const &opts)
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
                            OSVR_SHM_VERBOSE("Couldn't allocate buffer #"
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

            detail::IPCPutResultPtr produceElement() {
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
                OSVR_SHM_VERBOSE(
                    "Attempting to get an exclusive lock on sequence "
                    << sequenceNumber << " aka index " << back(lock));
#endif
                auto elementLock = back(lock)->getExclusiveLock();
                /// shared memory nullptr filled in by outer class
                detail::IPCPutResultPtr ret(new detail::IPCPutResult{
                    back(lock)->getBuf(elementLock), sequenceNumber,
                    std::move(elementLock), std::move(lock), nullptr});
                return ret;
            }

          private:
            raw_index_type m_capacity;
            ipc_offset_ptr<ElementData> elementArray;
            sequence_type m_beginSequenceNumber;
            sequence_type m_nextSequenceNumber;
            raw_index_type m_begin;
            raw_index_type m_size;
            uint32_t m_bufLen;
        };

        static size_t
        computeRequiredSpace(SharedMemoryRingBuffer::Options const &opts) {
            size_t alignedEntrySize = opts.getEntrySize() + opts.getAlignment();
            size_t dataSize = alignedEntrySize * (opts.getEntries() + 1);
            // Give 33% overhead on the raw bookkeeping data
            static const size_t BOOKKEEPING_SIZE =
                (sizeof(Bookkeeping) +
                 (sizeof(ElementData) * opts.getEntries())) *
                4 / 3;
            return dataSize + BOOKKEEPING_SIZE;
        }

        class SharedMemorySegmentHolder {
          public:
            SharedMemorySegmentHolder() : m_bookkeeping(nullptr) {}
            virtual ~SharedMemorySegmentHolder(){};

            Bookkeeping *getBookkeeping() { return m_bookkeeping; }

            virtual uint64_t getSize() const = 0;
            virtual uint64_t getFreeMemory() const = 0;

          protected:
            Bookkeeping *m_bookkeeping;
        };

        template <typename ManagedMemory>
        class SegmentHolderBase : public SharedMemorySegmentHolder {
          public:
            typedef ManagedMemory managed_memory_type;

            virtual uint64_t getSize() const { return m_shm->get_size(); }
            virtual uint64_t getFreeMemory() const {
                return m_shm->get_free_memory();
            }

          protected:
            unique_ptr<managed_memory_type> m_shm;
        };
        template <typename ManagedMemory>
        class ServerSharedMemorySegmentHolder
            : public SegmentHolderBase<ManagedMemory> {
          public:
            typedef SegmentHolderBase<ManagedMemory> Base;
            ServerSharedMemorySegmentHolder(
                SharedMemoryRingBuffer::Options const &opts)
                : m_name(opts.getName()) {
                OSVR_SHM_VERBOSE("Creating segment, name "
                                 << opts.getName() << ", size "
                                 << computeRequiredSpace(opts));
                try {
                    removeSharedMemory();
                    /// @todo Some shared memory types (specifically, Windows),
                    /// don't have a remove, so we should re-open.
                    Base::m_shm.reset(new ManagedMemory(
                        bip::create_only, opts.getName().c_str(),
                        computeRequiredSpace(opts)));
                } catch (bip::interprocess_exception &e) {
                    OSVR_SHM_VERBOSE("Failed to create shared memory segment "
                                     << opts.getName()
                                     << " with exception: " << e.what());
                    return;
                }
                // destroy_unique_instance<Bookkeeping>(*m_shm);
                Base::m_bookkeeping =
                    Base::m_shm->template construct<Bookkeeping>(
                        bip::unique_instance)(*Base::m_shm, opts);
            }

            virtual ~ServerSharedMemorySegmentHolder() {
                if (Base::m_bookkeeping) {
                    Base::m_bookkeeping->freeBufs(*Base::m_shm);
                }
                removeSharedMemory();
            }

            void removeSharedMemory() {
                ipc::device_type<ManagedMemory>::remove(m_name.c_str());
            }

          private:
            std::string m_name;
        };

        template <typename ManagedMemory>
        class ClientSharedMemorySegmentHolder
            : public SegmentHolderBase<ManagedMemory> {
          public:
            typedef SegmentHolderBase<ManagedMemory> Base;
            ClientSharedMemorySegmentHolder(
                SharedMemoryRingBuffer::Options const &opts) {
                OSVR_SHM_VERBOSE("Finding segment, name " << opts.getName());
                try {
                    Base::m_shm.reset(new ManagedMemory(
                        bip::open_only, opts.getName().c_str()));
                } catch (bip::interprocess_exception &e) {
                    OSVR_SHM_VERBOSE("Failed to open shared memory segment "
                                     << opts.getName()
                                     << " with exception: " << e.what());
                    return;
                }
                auto bookkeeping = Base::m_shm->template find<Bookkeeping>(
                    bip::unique_instance);
                Base::m_bookkeeping = bookkeeping.first;
            }

            virtual ~ClientSharedMemorySegmentHolder() {}

          private:
        };

        /// @brief Factory function for constructing a memory segment holder.
        template <typename ManagedMemory>
        inline unique_ptr<SharedMemorySegmentHolder>
        constructMemorySegment(SharedMemoryRingBuffer::Options const &opts,
                               bool doCreate) {
            unique_ptr<SharedMemorySegmentHolder> ret;
            if (doCreate) {
                ret.reset(
                    new ServerSharedMemorySegmentHolder<ManagedMemory>(opts));
            } else {
                ret.reset(
                    new ClientSharedMemorySegmentHolder<ManagedMemory>(opts));
            }
            if (nullptr == ret->getBookkeeping()) {
                ret.reset();
            } else {
                OSVR_SHM_VERBOSE("size: " << ret->getSize() << ", free: "
                                          << ret->getFreeMemory());
            }
            return ret;
        }
    } // namespace

    SharedMemoryRingBuffer::BufferWriteProxy::BufferWriteProxy(
        detail::IPCPutResultPtr &&data, SharedMemoryRingBufferPtr &&shm)
        : m_buf(nullptr), m_seq(0), m_data(std::move(data)) {
        if (m_data) {
            m_buf = m_data->buffer;
            m_seq = m_data->seq;
            m_data->shm = std::move(shm);
        }
    }

    SharedMemoryRingBuffer::BufferReadProxy::BufferReadProxy(
        detail::IPCGetResultPtr &&data, SharedMemoryRingBufferPtr &&shm)
        : m_buf(nullptr), m_seq(0), m_data(std::move(data)) {
        if (nullptr != m_data) {
            m_buf = m_data->buffer;
            m_seq = m_data->seq;
            m_data->shm = std::move(shm);
        }
    }

    SharedMemoryRingBuffer::smart_pointer_type SharedMemoryRingBuffer::BufferReadProxy::getBufferSmartPointer() const {
        return smart_pointer_type(m_data, m_buf);
    }

    SharedMemoryRingBuffer::Options::Options()
        : m_shmBackend(ipc::DEFAULT_MANAGED_SHM_ID) {}

    SharedMemoryRingBuffer::Options::Options(std::string const &name)
        : m_name(ipc::make_name_safe(name)),
          m_shmBackend(ipc::DEFAULT_MANAGED_SHM_ID) {}

    SharedMemoryRingBuffer::Options::Options(std::string const &name,
                                             BackendType backend)
        : m_name(ipc::make_name_safe(name)), m_shmBackend(backend) {}

    SharedMemoryRingBuffer::Options &
    SharedMemoryRingBuffer::Options::setName(std::string const &name) {
        m_name = ipc::make_name_safe(name);
        return *this;
    }

    SharedMemoryRingBuffer::Options &
    SharedMemoryRingBuffer::Options::setAlignment(alignment_type alignment) {
        /// @todo ensure power of 2
        m_alignment = alignment;
        return *this;
    }

    SharedMemoryRingBuffer::Options &
    SharedMemoryRingBuffer::Options::setEntries(entry_count_type entries) {
        m_entries = entries;
        return *this;
    }

    SharedMemoryRingBuffer::Options &
    SharedMemoryRingBuffer::Options::setEntrySize(entry_size_type entrySize) {
        m_entrySize = entrySize;
        return *this;
    }
    class SharedMemoryRingBuffer::Impl {
      public:
        Impl(unique_ptr<SharedMemorySegmentHolder> &&segment,
             Options const &opts)
            : m_seg(std::move(segment)), m_bookkeeping(nullptr), m_opts(opts) {
            m_bookkeeping = m_seg->getBookkeeping();
            m_opts.setEntries(m_bookkeeping->getCapacity());
            m_opts.setEntrySize(m_bookkeeping->getBufferLength());
        }

        detail::IPCPutResultPtr put() {
            return m_bookkeeping->produceElement();
        }

        detail::IPCGetResultPtr get(sequence_type num) {
            detail::IPCGetResultPtr ret;
            auto boundsLock = m_bookkeeping->getSharableLock();
            auto elt = m_bookkeeping->getBySequenceNumber(num, boundsLock);
            if (nullptr != elt) {
                auto readerLock = elt->getSharableLock();
                auto buf = elt->getBuf(readerLock);
                /// The nullptr will be filled in by the main object.
                ret.reset(new detail::IPCGetResult{buf, std::move(readerLock),
                                                   num, nullptr});
            }
            return ret;
        }

        detail::IPCGetResultPtr getLatest() {
            detail::IPCGetResultPtr ret;
            auto boundsLock = m_bookkeeping->getSharableLock();
            auto elt = m_bookkeeping->back(boundsLock);
            if (nullptr != elt) {
                auto readerLock = elt->getSharableLock();
                auto buf = elt->getBuf(readerLock);
                /// The nullptr will be filled in by the main object.
                ret.reset(new detail::IPCGetResult{
                    buf, std::move(readerLock),
                    m_bookkeeping->backSequenceNumber(boundsLock), nullptr});
            }
            return ret;
        }

        Options const &getOpts() const { return m_opts; }

      private:
        unique_ptr<SharedMemorySegmentHolder> m_seg;
        Bookkeeping *m_bookkeeping;

        Options m_opts;
    };

    SharedMemoryRingBufferPtr
    SharedMemoryRingBuffer::m_constructorHelper(Options const &opts,
                                                bool doCreate) {

        SharedMemoryRingBufferPtr ret;
        unique_ptr<SharedMemorySegmentHolder> segment;

        switch (opts.getBackend()) {

        case ipc::BASIC_MANAGED_SHM_ID:
            segment =
                constructMemorySegment<ipc::basic_managed_shm>(opts, doCreate);
            break;

#ifdef BOOST_INTERPROCESS_WINDOWS
        case ipc::WINDOWS_MANAGED_SHM_ID:
            segment = constructMemorySegment<ipc::windows_managed_shm>(
                opts, doCreate);
            break;
#endif

#ifdef BOOST_INTERPROCESS_XSI_SHARED_MEMORY_OBJECTS
        case ipc::SYSV_MANAGED_SHM_ID:
            segment =
                constructMemorySegment<ipc::sysv_managed_shm>(opts, doCreate);
            break;
#endif

        default:
            OSVR_SHM_VERBOSE("Unsupported/unrecognized shared memory backend: "
                             << int(opts.getBackend()));
            break;
        }

        if (!segment) {
            return ret;
        }
        unique_ptr<Impl> impl(new Impl(std::move(segment), opts));
        ret.reset(new SharedMemoryRingBuffer(std::move(impl)));
        return ret;
    }

    SharedMemoryRingBuffer::abi_level_type
    SharedMemoryRingBuffer::getABILevel() {
        return SHM_SOURCE_ABI_LEVEL;
    }

    SharedMemoryRingBufferPtr
    SharedMemoryRingBuffer::create(Options const &opts) {
        return m_constructorHelper(opts, true);
    }
    SharedMemoryRingBufferPtr
    SharedMemoryRingBuffer::find(Options const &opts) {
        return m_constructorHelper(opts, false);
    }

    SharedMemoryRingBuffer::SharedMemoryRingBuffer(unique_ptr<Impl> &&impl)
        : m_impl(std::move(impl)) {}

    SharedMemoryRingBuffer::~SharedMemoryRingBuffer() {}

    SharedMemoryRingBuffer::BackendType
    SharedMemoryRingBuffer::getBackend() const {
        return m_impl->getOpts().getBackend();
    }

    std::string const &SharedMemoryRingBuffer::getName() const {
        return m_impl->getOpts().getName();
    }

    uint32_t SharedMemoryRingBuffer::getEntrySize() const {
        return m_impl->getOpts().getEntrySize();
    }

    uint16_t SharedMemoryRingBuffer::getEntries() const {
        return m_impl->getOpts().getEntries();
    }

    SharedMemoryRingBuffer::BufferWriteProxy SharedMemoryRingBuffer::put() {
        return BufferWriteProxy(m_impl->put(), shared_from_this());
    }

    sequence_type SharedMemoryRingBuffer::put(pointer_to_const_type data,
                                              size_t len) {
        auto proxy = put();
        std::memcpy(proxy.get(), data, len);
        return proxy.getSequenceNumber();
    }

    SharedMemoryRingBuffer::BufferReadProxy
    SharedMemoryRingBuffer::get(sequence_type num) {
        return BufferReadProxy(m_impl->get(num), shared_from_this());
    }

    SharedMemoryRingBuffer::BufferReadProxy
    SharedMemoryRingBuffer::getLatest() {
        return BufferReadProxy(m_impl->getLatest(), shared_from_this());
    }

} // namespace common
} // namespace osvr
