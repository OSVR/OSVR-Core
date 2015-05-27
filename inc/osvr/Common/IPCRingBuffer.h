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

#ifndef INCLUDED_IPCRingBuffer_h_GUID_4F33BDA7_1BEB_4E81_B96C_1ADA1CBD1997
#define INCLUDED_IPCRingBuffer_h_GUID_4F33BDA7_1BEB_4E81_B96C_1ADA1CBD1997

// Internal Includes
#include <osvr/Common/Export.h>
#include <osvr/Util/StdInt.h>
#include <osvr/Util/UniquePtr.h>
#include <osvr/Util/SharedPtr.h>

// Library/third-party includes
// - none

// Standard includes
#include <string>

namespace osvr {
namespace common {
    namespace detail {
        struct IPCPutResult;
        typedef shared_ptr<IPCPutResult> IPCPutResultPtr;
        struct IPCGetResult;
        typedef shared_ptr<IPCGetResult> IPCGetResultPtr;
    }
    // forward declaration
    class IPCRingBuffer;

    /// @brief Pointer type for holding a shared memory ring buffer.
    typedef shared_ptr<IPCRingBuffer> IPCRingBufferPtr;

    /// @brief A ring buffer for interprocess communication, with managed
    /// occupancy.
    ///
    /// Each element in the ring buffer (that is, the contained buffers) is
    /// aligned to the given power of 2 alignment.
    ///
    /// Designed to provide large-format data transfer in single-producer,
    /// roughly broadcast model, with an outside channel for communicating the
    /// segment name and signalling new data, and no guarantee that the data you
    /// were notified about won't be overwritten - just that if you're currently
    /// accessing data, we won't overwrite that.
    class IPCRingBuffer : public enable_shared_from_this<IPCRingBuffer> {
      public:
        typedef uint8_t BackendType;
        typedef uint16_t alignment_type;
        typedef uint16_t entry_count_type;
        typedef uint32_t entry_size_type;
        typedef uint32_t abi_level_type;
        class Options {
          public:
            OSVR_COMMON_EXPORT Options();
            OSVR_COMMON_EXPORT Options(std::string const &name);

            OSVR_COMMON_EXPORT Options(std::string const &name,
                                       BackendType backend);

            BackendType getBackend() const { return m_shmBackend; }

            /// @brief sets the name, after sanitizing the input string.
            /// @return *this for chained method idiom.
            Options &setName(std::string const &name);
            std::string const &getName() const { return m_name; }

            /// @brief Sets the alignment for each entry, which must be a power
            /// of 2 (rounded up to the nearest if it's not).
            /// @return *this for chained method idiom.
            Options &setAlignment(alignment_type alignment);
            alignment_type getAlignment() const { return m_alignment; }

            /// @brief Sets the number of entries in the ring buffer.
            /// @return *this for chained method idiom.
            Options &setEntries(entry_count_type entries);
            entry_count_type getEntries() const { return m_entries; }

            /// @brief Sets the size of each entry in the ring buffer.
            /// @return *this for chained method idiom.
            Options &setEntrySize(entry_size_type entrySize);
            entry_size_type getEntrySize() const { return m_entrySize; }

          private:
            std::string m_name;
            BackendType m_shmBackend;
            alignment_type m_alignment = 16;
            entry_count_type m_entries = 16;
            entry_size_type m_entrySize = 65536;
        };

        /// @brief Gets an integer representing a unique arrangement of the
        /// internal shared memory layout, such that if two processes try to
        /// communicate with different ABI levels, they will (likely) not
        /// succeed and thus should not try.
        OSVR_COMMON_EXPORT static abi_level_type getABILevel();

        /// @brief Named constructor, for use by server processes: creates a
        /// shared memory ring buffer given the options structure.
        ///
        /// If the returned pointer is not valid, the named segment could not be
        /// created for some reason.
        OSVR_COMMON_EXPORT static IPCRingBufferPtr create(Options const &opts);

        /// @brief Named constructor, for use by client processes: accesses an
        /// IPC ring buffer using the options structure. Only the name field is
        /// used from the options.
        ///
        /// If the returned pointer is not valid, the named buffer could not be
        /// found.
        OSVR_COMMON_EXPORT static IPCRingBufferPtr find(Options const &opts);

        /// @brief Returns an integer identifying the IPC backend used.
        OSVR_COMMON_EXPORT BackendType getBackend() const;

        /// @brief Returns the name string used to create or find this ring
        /// buffer
        OSVR_COMMON_EXPORT std::string const &getName() const;

        /// @brief Returns the size of each individual buffer entry, in bytes.
        OSVR_COMMON_EXPORT uint32_t getEntrySize() const;

        /// @brief Returns the total capacity, in number of buffer entries, of
        /// this ring buffer.
        OSVR_COMMON_EXPORT uint16_t getEntries() const;

        /// @brief The sequence number is automatically incremented with each
        /// "put" into the buffer. Note that, as an unsigned integer, it does
        /// have (and uses) well-defined overflow semantics.
        typedef uint32_t sequence_type;

        typedef uint8_t value_type;
        typedef value_type *pointer_type;
        typedef value_type const *pointer_to_const_type;

        typedef shared_ptr<value_type> smart_pointer_type;

        /// @brief A class providing write access to the next available element
        /// in the ring buffer, owning the appropriate mutex locks and providing
        /// access to the sequence number.
        class BufferWriteProxy {
          public:
            /// @brief not copyable
            BufferWriteProxy(BufferWriteProxy const &) = delete;

            /// @brief not copy-assignable
            BufferWriteProxy &operator=(BufferWriteProxy const &) = delete;

            /// @brief move-constructible
            BufferWriteProxy(BufferWriteProxy &&other) {
                std::swap(m_data, other.m_data);
            }

            /// @brief move-assignable
            BufferWriteProxy &operator=(BufferWriteProxy &&other) {
                std::swap(m_data, other.m_data);
                return *this;
            }

            operator pointer_type() const { return get(); }

            pointer_type get() const { return m_buf; }

            sequence_type getSequenceNumber() const { return m_seq; }

          private:
            BufferWriteProxy(detail::IPCPutResultPtr &&data,
                             IPCRingBufferPtr &&shm);
            friend class IPCRingBuffer;
            pointer_type m_buf;
            sequence_type m_seq;
            detail::IPCPutResultPtr m_data;
        };

        /// @brief A class providing access to an entry in the ring buffer,
        /// holding a sharable mutex lock preventing it from being overwritten
        /// while this object is in scope.
        ///
        /// As such, you should only access the memory pointed to by this object
        /// while you keep this object alive, and you should let it go out of
        /// scope when you no longer need the data.
        ///
        /// This object acts somewhat like a smart pointer.
        class BufferReadProxy {
          public:
            /// @brief Checks validity of pointer - was the specified buffer
            /// entry available?
            explicit operator bool() const { return nullptr != m_buf; }

            /// @brief Gets the raw pointer
            pointer_to_const_type get() const { return m_buf; }

            /// @brief Gets a smart pointer to the buffer that shares ownership
            /// of the underlying resources of this object.
            smart_pointer_type getBufferSmartPointer() const;

            /// @brief Gets the sequence number associated with this entry.
            sequence_type getSequenceNumber() const { return m_seq; }

            pointer_to_const_type operator*() const { return m_buf; }
            pointer_to_const_type operator->() const { return m_buf; }

          private:
            BufferReadProxy(detail::IPCGetResultPtr &&data,
                            IPCRingBufferPtr &&shm);
            friend class IPCRingBuffer;
            pointer_type m_buf;
            sequence_type m_seq;
            detail::IPCGetResultPtr m_data;
        };

        /// @brief Puts the data in the next element in the buffer (using
        /// memcpy). Buffer sizes are not checked!
        ///
        /// This is a convenience wrapper around the other put() signature.
        OSVR_COMMON_EXPORT sequence_type
        put(pointer_to_const_type data, size_t len);

        /// @brief Gets a proxy object for putting data in the next element in
        /// the buffer. You're responsible for doing the copying and, once you
        /// let the returned object exit scope, the notification (possibly with
        /// sequence number)
        OSVR_COMMON_EXPORT BufferWriteProxy put();

        /// @brief Gets access to an element in the buffer by sequence number:
        /// returns a proxy object  that behaves mostly like a smart pointer.
        OSVR_COMMON_EXPORT BufferReadProxy get(sequence_type num);

        /// @brief Gets access to the most recent element in the buffer: returns
        /// a proxy object that behaves mostly like a smart pointer, and that
        /// also contains the associated sequence number.
        OSVR_COMMON_EXPORT BufferReadProxy getLatest();

        /// @brief Destructor.
        OSVR_COMMON_EXPORT ~IPCRingBuffer();

      private:
        static IPCRingBufferPtr m_constructorHelper(Options const &opts,
                                                    bool doCreate);
        class Impl;
        IPCRingBuffer(unique_ptr<Impl> &&impl);
        unique_ptr<Impl> m_impl;
    };

} // namespace common
} // namespace osvr

#endif // INCLUDED_IPCRingBuffer_h_GUID_4F33BDA7_1BEB_4E81_B96C_1ADA1CBD1997
