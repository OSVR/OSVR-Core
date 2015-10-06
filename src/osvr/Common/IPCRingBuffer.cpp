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
#include <osvr/Common/IPCRingBuffer.h>
#include "IPCRingBufferResults.h"
#include "IPCRingBufferSharedObjects.h"
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
#define OSVR_SHM_VERBOSE(X) OSVR_DEV_VERBOSE("IPCRingBuffer: " << X)

    /// @brief the ABI level: this must be bumped if the layout of any
    /// shared-memory objects (Bookkeeping, ElementData) changes, if Boost
    /// Interprocess changes affect the utilized ABI, or if other changes occur
    /// that would interfere with communication.
    static IPCRingBuffer::abi_level_type SHM_SOURCE_ABI_LEVEL = 0;

/// Some tests that can be automated for ensuring validity of the ABI level
/// number.
#if (BOOST_VERSION > 105900)
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

    static_assert(std::is_same<IPCRingBuffer::BackendType,
                               ipc::SharedMemoryBackendType>::value,
                  "The typedefs IPCRingBuffer::BackendType and "
                  "ipc::SharedMemoryBackendType must remain in sync!");
    static_assert(
        std::is_same<IPCRingBuffer::value_type, OSVR_ImageBufferElement>::value,
        "The ring buffer's individual byte type must match the image buffer "
        "element type.");

    namespace bip = boost::interprocess;
    namespace {
        typedef OSVR_ImageBufferElement BufferType;

        template <typename T, typename ManagedMemory>
        using ipc_deleter_type =
            typename ManagedMemory::template deleter<T>::type;

        typedef IPCRingBuffer::sequence_type sequence_type;

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

    namespace {

        static size_t computeRequiredSpace(IPCRingBuffer::Options const &opts) {
            size_t alignedEntrySize = opts.getEntrySize() + opts.getAlignment();
            size_t dataSize = alignedEntrySize * (opts.getEntries() + 1);
            // Give 33% overhead on the raw bookkeeping data
            static const size_t BOOKKEEPING_SIZE =
                (sizeof(detail::Bookkeeping) +
                 (sizeof(detail::ElementData) * opts.getEntries())) *
                4 / 3;
            return dataSize + BOOKKEEPING_SIZE;
        }

        class SharedMemorySegmentHolder {
          public:
            SharedMemorySegmentHolder() : m_bookkeeping(nullptr) {}
            virtual ~SharedMemorySegmentHolder(){};

            detail::Bookkeeping *getBookkeeping() { return m_bookkeeping; }

            virtual uint64_t getSize() const = 0;
            virtual uint64_t getFreeMemory() const = 0;

          protected:
            detail::Bookkeeping *m_bookkeeping;
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
            ServerSharedMemorySegmentHolder(IPCRingBuffer::Options const &opts)
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
                // detail::Bookkeeping::destroy(*Base::m_shm);
                Base::m_bookkeeping =
                    detail::Bookkeeping::construct(*Base::m_shm, opts);
            }

            virtual ~ServerSharedMemorySegmentHolder() {
                detail::Bookkeeping::destroy(*Base::m_shm);
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
                IPCRingBuffer::Options const &opts) {
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
                Base::m_bookkeeping = detail::Bookkeeping::find(*Base::m_shm);
            }

            virtual ~ClientSharedMemorySegmentHolder() {}

          private:
        };

        /// @brief Factory function for constructing a memory segment holder.
        template <typename ManagedMemory>
        inline unique_ptr<SharedMemorySegmentHolder>
        constructMemorySegment(IPCRingBuffer::Options const &opts,
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

    IPCRingBuffer::BufferWriteProxy::BufferWriteProxy(
        detail::IPCPutResultPtr &&data, IPCRingBufferPtr &&shm)
        : m_buf(nullptr), m_seq(0), m_data(std::move(data)) {
        if (m_data) {
            m_buf = m_data->buffer;
            m_seq = m_data->seq;
            m_data->shm = std::move(shm);
        }
    }

    IPCRingBuffer::BufferReadProxy::BufferReadProxy(
        detail::IPCGetResultPtr &&data, IPCRingBufferPtr &&shm)
        : m_buf(nullptr), m_seq(0), m_data(std::move(data)) {
        if (nullptr != m_data) {
            m_buf = m_data->buffer;
            m_seq = m_data->seq;
            m_data->shm = std::move(shm);
        }
    }

    IPCRingBuffer::smart_pointer_type
    IPCRingBuffer::BufferReadProxy::getBufferSmartPointer() const {
        return smart_pointer_type(m_data, m_buf);
    }

    IPCRingBuffer::Options::Options()
        : m_shmBackend(ipc::DEFAULT_MANAGED_SHM_ID) {}

    IPCRingBuffer::Options::Options(std::string const &name)
        : m_name(ipc::make_name_safe(name)),
          m_shmBackend(ipc::DEFAULT_MANAGED_SHM_ID) {}

    IPCRingBuffer::Options::Options(std::string const &name,
                                    BackendType backend)
        : m_name(ipc::make_name_safe(name)), m_shmBackend(backend) {}

    IPCRingBuffer::Options &
    IPCRingBuffer::Options::setName(std::string const &name) {
        m_name = ipc::make_name_safe(name);
        return *this;
    }

    IPCRingBuffer::Options &
    IPCRingBuffer::Options::setAlignment(alignment_type alignment) {
        /// @todo ensure power of 2
        m_alignment = alignment;
        return *this;
    }

    IPCRingBuffer::Options &
    IPCRingBuffer::Options::setEntries(entry_count_type entries) {
        m_entries = entries;
        return *this;
    }

    IPCRingBuffer::Options &
    IPCRingBuffer::Options::setEntrySize(entry_size_type entrySize) {
        m_entrySize = entrySize;
        return *this;
    }
    class IPCRingBuffer::Impl {
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
        detail::Bookkeeping *m_bookkeeping;

        Options m_opts;
    };

    IPCRingBufferPtr IPCRingBuffer::m_constructorHelper(Options const &opts,
                                                        bool doCreate) {

        IPCRingBufferPtr ret;
        unique_ptr<SharedMemorySegmentHolder> segment;

        switch (opts.getBackend()) {

        case ipc::BASIC_MANAGED_SHM_ID:
            segment =
                constructMemorySegment<ipc::basic_managed_shm>(opts, doCreate);
            break;

#ifdef OSVR_HAVE_WINDOWS_SHM
        case ipc::WINDOWS_MANAGED_SHM_ID:
            segment = constructMemorySegment<ipc::windows_managed_shm>(
                opts, doCreate);
            break;
#endif

#ifdef OSVR_HAVE_XSI_SHM
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
        ret.reset(new IPCRingBuffer(std::move(impl)));
        return ret;
    }

    IPCRingBuffer::abi_level_type IPCRingBuffer::getABILevel() {
        return SHM_SOURCE_ABI_LEVEL;
    }

    IPCRingBufferPtr IPCRingBuffer::create(Options const &opts) {
        return m_constructorHelper(opts, true);
    }
    IPCRingBufferPtr IPCRingBuffer::find(Options const &opts) {
        return m_constructorHelper(opts, false);
    }

    IPCRingBuffer::IPCRingBuffer(unique_ptr<Impl> &&impl)
        : m_impl(std::move(impl)) {}

    IPCRingBuffer::~IPCRingBuffer() {}

    IPCRingBuffer::BackendType IPCRingBuffer::getBackend() const {
        return m_impl->getOpts().getBackend();
    }

    std::string const &IPCRingBuffer::getName() const {
        return m_impl->getOpts().getName();
    }

    uint32_t IPCRingBuffer::getEntrySize() const {
        return m_impl->getOpts().getEntrySize();
    }

    uint16_t IPCRingBuffer::getEntries() const {
        return m_impl->getOpts().getEntries();
    }

    IPCRingBuffer::BufferWriteProxy IPCRingBuffer::put() {
        return BufferWriteProxy(m_impl->put(), shared_from_this());
    }

    IPCRingBuffer::sequence_type IPCRingBuffer::put(pointer_to_const_type data,
                                                    size_t len) {
        auto proxy = put();
        std::memcpy(proxy.get(), data, len);
        return proxy.getSequenceNumber();
    }

    IPCRingBuffer::BufferReadProxy IPCRingBuffer::get(sequence_type num) {
        return BufferReadProxy(m_impl->get(num), shared_from_this());
    }

    IPCRingBuffer::BufferReadProxy IPCRingBuffer::getLatest() {
        return BufferReadProxy(m_impl->getLatest(), shared_from_this());
    }

} // namespace common
} // namespace osvr
