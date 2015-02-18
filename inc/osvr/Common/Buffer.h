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
// - none

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

    /// @brief A typedef for a buffer (a vector of bytes), that might be
    /// "over-aligned" in some desirable way.
    ///
    /// Check ActualBufferAlignment::value to see if it is actually aligned.
    typedef std::vector<BufferElement, BufferAllocator> Buffer;

    inline size_t computeAlignmentPadding(size_t alignment,
                                          size_t currentSize) {
        auto leftover = currentSize % alignment;
        return (leftover == 0) ? 0 : (alignment - leftover);
    }

    template <typename BufferType = Buffer> class BufferWrapper {
      public:
        class Reader {
          public:
            size_t bytesRead() const { return m_readIter - m_begin; }
            size_t remaining() const { return m_end - m_readIter; }
            /// @brief Get the binary representation of a type from a buffer
            template <typename T> void read(T &v) {
                if (remaining() < sizeof(T)) {
                    throw std::runtime_error(
                        "Not enough data in the buffer to read this type!");
                }
                ElementType const *dest =
                    reinterpret_cast<ElementType const *>(&v);

                auto readEnd = m_readIter + sizeof(T);
                std::copy(m_readIter, readEnd, dest);
                m_readIter = readEnd;
            }

            /// @brief Skip reading the given number of bytes, assumed to be
            /// padding.
            void skipPadding(size_t const bytes) {
                if (bytes == 0) {
                    return;
                }
                if (remaining() < bytes) {
                    throw std::runtime_error("Can't skip that many padding "
                                             "bytes, not that many bytes "
                                             "left!");
                }
                m_readIter += bytes;
            }

          private:
            typedef typename BufferType::const_iterator const_iterator;
            Reader(BufferType const &buf)
                : m_buf(&buf), m_begin(m_buf->begin()),
                  m_readIter(m_buf->begin()), m_end(m_buf->end()) {}
            BufferType const *m_buf;
            const_iterator m_begin;
            const_iterator m_readIter;
            const_iterator m_end;
            typedef typename BufferType::value_type ElementType;
            friend class BufferWrapper;
        };

        typedef typename BufferType::value_type ElementType;

        /// @brief Constructs an empty buffer
        BufferWrapper(){};

        /// @brief Constructs a buffer wrapper by copy-constructing the
        /// contained buffer type.
        explicit BufferWrapper(BufferType const &buf) : m_buf(buf) {}

        /// @brief Append the binary representation of a value
        template <typename T> void append(T const v) {
            ElementType const *src = reinterpret_cast<ElementType const *>(&v);
            m_buf.insert(m_buf.end(), src, src + sizeof(T));
        }

        /// @brief Append a byte-array's contents
        void append(ElementType const *v, size_t n) {
            m_buf.insert(m_buf.end(), v, v + n);
        }

        /// @brief Append the specified number of bytes of padding.
        void appendPadding(size_t const bytes) {
            if (bytes == 0) {
                return;
            }
            m_buf.insert(m_buf.end(), bytes, '\0');
        }

        /// @brief Returns a reader object, for making a single read pass over
        /// the buffer. Do not modify this buffer during the lifetime of a
        /// reader!
        Reader startReading() const { return Reader(m_buf); }

        typedef typename BufferType::const_iterator const_iterator;

        const_iterator begin() const { return m_buf.begin(); }

        const_iterator end() const { return m_buf.end(); }

        size_t size() const { return m_buf.size(); }

      private:
        BufferType m_buf;
    };

    /// @brief Push the binary representation of a type on to a buffer
    template <typename T> inline void bufferAppend(Buffer &buf, T const v) {
        BufferElement const *src = reinterpret_cast<BufferElement const *>(&v);
        buf.insert(buf.end(), src, src + sizeof(T));
    }

    /// @brief Get the binary representation of a type from a buffer
    template <typename T>
    inline void bufferRead(Buffer const &buf, Buffer::const_iterator &it,
                           Buffer::const_iterator const &end, T &v) {
        if (end - it < sizeof(T)) {
            throw std::runtime_error(
                "Not enough data in the buffer to read this type!");
        }
        BufferElement const *dest = reinterpret_cast<BufferElement const *>(&v);
        std::copy(it, it + sizeof(T), dest);
        it += sizeof(T);
    }
} // namespace common
} // namespace osvr
#endif // INCLUDED_Buffer_h_GUID_55EB8AAF_57A7_49A4_3A3A_49293A72211D
