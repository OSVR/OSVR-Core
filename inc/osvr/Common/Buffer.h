/** @file
    @brief Header defining buffer types, with optional alignment dependent on
   Boost version.

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

#ifndef INCLUDED_Buffer_h_GUID_55EB8AAF_57A7_49A4_3A3A_49293A72211D
#define INCLUDED_Buffer_h_GUID_55EB8AAF_57A7_49A4_3A3A_49293A72211D

// Internal Includes
#include <osvr/Common/Buffer_fwd.h>

// Library/third-party includes
#include <boost/version.hpp>

#if BOOST_VERSION >= 105600
#include <boost/align/aligned_allocator.hpp>
#endif

// Standard includes
#include <vector>
#include <stdexcept>
#include <algorithm>

namespace osvr {
namespace common {
    /// @brief The single-byte element in a buffer.
    typedef char BufferElement;

    /// @brief Traits class specifying the (potentially platform-specific!)
    /// preferred alignment for Buffer.
    struct DesiredBufferAlignment {
        /// @brief Alignment in bytes
        static const size_t value = 16;
    };
#if BOOST_VERSION >= 105600
    // New Boost has code to handle aligned allocation. Alignment is hard to do
    // portably so we are happy to delegate.

    /// @brief Traits class describing actual alignment of Buffer.
    struct ActualBufferAlignment : DesiredBufferAlignment {};

    /// @brief Allocator type for Buffer - might be usefully aligned.
    typedef boost::alignment::aligned_allocator<
        BufferElement, ::osvr::common::DesiredBufferAlignment::value>
        BufferAllocator;

#else
    // Fallback for older boost: unaligned buffer. Want to rely on aligned
    // buffers? Upgrade Boost and rebuild.

    /// @brief Traits class describing actual alignment of Buffer.
    struct ActualBufferAlignment {
        static const size_t value = 1;
    };

    /// @brief Allocator type for Buffer - might be usefully aligned.
    typedef std::allocator<BufferElement> BufferAllocator;
#endif

    /// @brief A typedef for a vector of bytes usable as a buffer, that might be
    /// "over-aligned" in some desirable way.
    ///
    /// Check ActualBufferAlignment::value to see if it is actually aligned.
    typedef std::vector<BufferElement, BufferAllocator> BufferByteVector;

    /// @brief Given an alignment in bytes, and a current size of a buffer,
    /// return the number of bytes of padding required to align the next field
    /// to be added to the buffer at the desired alignment within that buffer.
    ///
    /// That is, return some padding such that (currentSize + padding) %
    /// alignment == 0 for alignment > 1.
    ///
    /// @param alignment Alignment in bytes: both 0 and 1 are accepted to mean
    /// "no alignment"
    /// @param currentSize Current number of bytes in a buffer
    inline size_t computeAlignmentPadding(size_t alignment,
                                          size_t currentSize) {
        size_t ret = 0;
        if (2 > alignment) {
            /// No alignment requested
            return ret;
        }
        auto leftover = currentSize % alignment;
        if (leftover == 0) {
            /// Buffer is already aligned
            return ret;
        }
        /// Buffer needs some padding
        ret = alignment - leftover;
        return ret;
    }

    /// @brief Provides for a single reading pass over a buffer. It is
    /// important that the buffer not change while a Reader obtained from it
    /// is still in scope.
    ///
    /// Get one by calling Buffer::startReading()
    template <typename ContainerType> class BufferReader {
      public:
        typedef typename ContainerType::const_iterator const_iterator;
        typedef typename ContainerType::value_type ElementType;

        size_t bytesRead() const { return m_readIter - m_begin; }
        size_t bytesRemaining() const { return m_end - m_readIter; }
        /// @brief Get the binary representation of a type from a buffer
        template <typename T> void read(T &v) {
            if (bytesRemaining() < sizeof(T)) {
                throw std::runtime_error(
                    "Not enough data in the buffer to read this type!");
            }
            /// Safe to do without violating strict aliasing because
            /// ElementType is a character type.
            ElementType *dest = reinterpret_cast<ElementType *>(&v);

            auto readEnd = m_readIter + sizeof(T);
            std::copy(m_readIter, readEnd, dest);
            m_readIter = readEnd;
        }

        /// @brief Returns an iterator into the buffer valid for n elements,
        /// and assumes you'll take care of copying them.
        const_iterator readBytes(size_t const n) {
            if (bytesRemaining() < n) {
                throw std::runtime_error(
                    std::string("Not enough data in the buffer to read ") + n +
                    " bytes!");
            }
            auto ret = m_readIter;
            m_readIter += n;
            return ret;
        }

        /// @brief Get the binary representation of a type from a buffer,
        /// after skipping the necessary number of bytes to begin the read
        /// at the given alignment.
        template <typename T> void readAligned(T &v, size_t const alignment) {
            skipPadding(computeAlignmentPadding(alignment, bytesRead()));
            read(v);
        }

        /// @brief Returns an iterator into the buffer valid for n elements,
        /// after skipping the necessary number of bytes to begin the read at
        /// the given alignment, and assumes you'll take care of copying them.
        const_iterator readBytesAligned(size_t const n,
                                        size_t const alignment) {
            skipPadding(computeAlignmentPadding(alignment, bytesRead()));
            return readBytes(n);
        }

        /// @brief Skip reading the given number of bytes, assumed to be
        /// padding.
        void skipPadding(size_t const bytes) {
            if (bytes == 0) {
                return;
            }
            if (bytesRemaining() < bytes) {
                throw std::runtime_error("Can't skip that many padding "
                                         "bytes, not that many bytes "
                                         "left!");
            }
            m_readIter += bytes;
        }

      private:
        BufferReader(ContainerType const &buf)
            : m_buf(&buf), m_begin(m_buf->begin()), m_readIter(m_buf->begin()),
              m_end(m_buf->end()) {}
        ContainerType const *m_buf;
        const_iterator m_begin;
        const_iterator m_readIter;
        const_iterator m_end;
        BOOST_STATIC_ASSERT_MSG(sizeof(ElementType) == 1,
                                "Container must have byte-sized elements");
        friend class Buffer<ContainerType>; ///< permits construction
    };

    /// @brief A buffer of bytes, built on a byte-vector-like container.
    /// Provides methods for easily appending to the buffer (including alignment
    /// padding), and a nested class for reading from such a buffer.
    template <typename ContainerType = BufferByteVector> class Buffer {
      public:
        /// @brief The (necessarily byte-size) type of elements in the
        /// underlying container.
        typedef typename ContainerType::value_type ElementType;

        /// @brief The corresponding BufferReader type.
        typedef BufferReader<ContainerType> Reader;

        /// @brief Constructs an empty buffer
        Buffer() {}

        /// @brief Constructs a buffer wrapper by copy-constructing the
        /// contained type.
        explicit Buffer(ContainerType const &buf) : m_buf(buf) {}

        /// @brief Constructs a buffer by copying from two input
        /// iterators, representing a half-open range [beginIt, endIt)
        ///
        /// Passed directly to contained type, so semantics are
        /// identical.
        template <typename InputIterator>
        Buffer(InputIterator beginIt, InputIterator endIt)
            : m_buf(beginIt, endIt) {}

        /// @brief Append the binary representation of a value
        ///
        /// The value cannot be a pointer here.
        template <typename T> void append(T const v) {
            BOOST_STATIC_ASSERT(!boost::is_pointer<T>::value);
            /// Safe to do without violating strict aliasing because ElementType
            /// is a character type.
            ElementType const *src = reinterpret_cast<ElementType const *>(&v);
            m_buf.insert(m_buf.end(), src, src + sizeof(T));
        }

        /// @brief Append the binary representation of a value, after adding the
        /// necessary number of padding bytes to begin the write at the given
        /// alignment within the buffer.
        ///
        /// The value cannot be a pointer here.
        template <typename T>
        void appendAligned(T const v, size_t const alignment) {
            appendPadding(computeAlignmentPadding(alignment, size()));
            append(v);
        }

        /// @brief Append a byte-array's contents
        void append(ElementType const *v, size_t const n) {
            m_buf.insert(m_buf.end(), v, v + n);
        }

        /// @brief Append a byte-array's contents, after adding the necessary
        /// number of padding bytes to begin the write at the given alignment
        /// within the buffer.
        void appendAligned(ElementType const *v, size_t const n,
                           size_t const alignment) {
            appendPadding(computeAlignmentPadding(alignment, size()));
            append(v, n);
        }

        /// @brief Append the specified number of bytes of padding.
        void appendPadding(size_t const bytes) {
            if (bytes == 0) {
                return;
            }
            m_buf.insert(m_buf.end(), bytes, PADDING_ELEMENT);
        }

        /// @brief Returns a reader object, for making a single read pass over
        /// the buffer. Do not modify this buffer during the lifetime of a
        /// reader!
        Reader startReading() const { return Reader(m_buf); }

#if 0
        /// @todo Commented out because YAGNI

        typedef typename ContainerType::const_iterator const_iterator;

        /// @brief Gets the "begin" iterator
        const_iterator begin() const { return m_buf.begin(); }

        /// @brief Gets the "past the end" iterator
        const_iterator end() const { return m_buf.end(); }
#endif
        /// @brief Gets the current size, in bytes.
        size_t size() const { return m_buf.size(); }

        /// @brief Provides access to the underlying container.
        ContainerType &getContents() { return m_buf; }

      private:
        static const ElementType PADDING_ELEMENT = '\0';
        ContainerType m_buf;
    };
} // namespace common
} // namespace osvr
#endif // INCLUDED_Buffer_h_GUID_55EB8AAF_57A7_49A4_3A3A_49293A72211D
