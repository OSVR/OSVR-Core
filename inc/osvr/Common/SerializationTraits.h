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

#ifndef INCLUDED_SerializationTraits_h_GUID_DF0CDFE0_097F_41B2_2DAE_7D4033D28D43
#define INCLUDED_SerializationTraits_h_GUID_DF0CDFE0_097F_41B2_2DAE_7D4033D28D43

// Internal Includes
#include <osvr/Common/AlignmentPadding.h>
#include <osvr/Common/Endianness.h>
#include <osvr/Common/SerializationTags.h>
#include <osvr/Util/BoolC.h>

// Library/third-party includes
#include <boost/call_traits.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/type_traits/alignment_of.hpp>
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>

namespace osvr {
namespace common {

    namespace serialization {

        /// @brief Traits class indicating how to serialize a type with a given
        /// tag. The default tag for a type `T` is `DefaultSerializationTag<T>`
        ///
        /// A traits class must implement:
        ///
        /// - `template <typename BufferType> static void
        /// serialize(BufferType &buf, typename Base::param_type val, Tag
        /// const&)`
        /// - `template <typename BufferReaderType> static void
        /// deserialize(BufferReaderType &buf, typename Base::reference_type
        /// val, Tag const&)`
        /// - `static size_t spaceRequired(size_t existingBytes, typename
        /// Base::param_type val, Tag const&)`
        ///
        /// The tag parameter may be used to pass runtime arguments in (like a
        /// length), if required.
        ///
        /// A traits class should inherit from `BaseSerializationTraits<T>` to
        /// get useful typedefs.
        ///
        /// See the serializers below for std::string for useful
        /// examples/starting points.
        ///
        /// The dummy template parameter exists for usage of `enable_if`.
        template <typename Tag, typename Dummy = void>
        struct SerializationTraits;

        /// @brief Serialize a value to a buffer, with optional tag to specify
        /// non-default traits.
        template <typename T, typename BufferType,
                  typename Tag = DefaultSerializationTag<T> >
        inline void serializeRaw(BufferType &buf, T const &v,
                                 Tag const &tag = Tag()) {
            SerializationTraits<Tag>::serialize(buf, v, tag);
        }

        /// @brief Deserialize a value from a buffer, with optional tag to
        /// specify non-default traits.
        template <typename T, typename BufferReaderType,
                  typename Tag = DefaultSerializationTag<T> >
        inline void deserializeRaw(BufferReaderType &reader, T &v,
                                   Tag const &tag = Tag()) {
            SerializationTraits<Tag>::deserialize(reader, v, tag);
        }

        /// @brief Get the size a value from a buffer, with optional tag to
        /// specify non-default traits.
        template <typename T, typename Tag = DefaultSerializationTag<T> >
        inline size_t getBufferSpaceRequiredRaw(size_t existingBufferSize,
                                                T const &v,
                                                Tag const &tag = Tag()) {
            return SerializationTraits<Tag>::spaceRequired(existingBufferSize,
                                                           v, tag);
        }

        /// @brief Base of serialization traits, containing useful typedefs.
        template <typename T> struct BaseSerializationTraits {
            typedef T type;
            typedef typename boost::call_traits<type>::param_type param_type;
            typedef typename boost::call_traits<type>::value_type value_type;
            typedef typename boost::call_traits<type>::reference reference_type;
        };

        /// @brief Serialization traits for a given arithmetic type (that is, a
        /// number type that has a network byte order) with a specified
        /// alignment
        template <typename T, size_t Alignment>
        struct ArithmeticSerializationTraits : BaseSerializationTraits<T> {
            typedef BaseSerializationTraits<T> Base;
            typedef T type;
            /// @brief Buffers an object of this type.
            template <typename BufferType, typename Tag>
            static void serialize(BufferType &buf,
                                  typename Base::param_type val, Tag const &) {
                buf.appendAligned(hton(val), Alignment);
            }

            /// @brief Reads an object of this type from a buffer
            template <typename BufferReaderType, typename Tag>
            static void deserialize(BufferReaderType &buf,
                                    typename Base::reference_type val,
                                    Tag const &) {
                buf.readAligned(val, Alignment);
                val = ntoh(val);
            }

            /// @brief Returns the number of bytes required for this type (and
            /// alignment padding if applicable) to be appended to a buffer of
            /// the supplied existing size.
            template <typename Tag>
            static size_t spaceRequired(size_t existingBytes,
                                        typename Base::param_type,
                                        Tag const &) {
                return computeAlignmentPadding(Alignment, existingBytes) +
                       sizeof(T);
            }
        };

        /// @brief Set up the default serialization traits for arithmetic types,
        /// aligning to their type size.
        template <typename T>
        struct SerializationTraits<
            DefaultSerializationTag<T>,
            typename std::enable_if<std::is_arithmetic<T>::value &&
                                    !std::is_same<bool, T>::value>::type>
            : ArithmeticSerializationTraits<T, sizeof(T)> {

            BOOST_STATIC_ASSERT(boost::alignment_of<T>::value == sizeof(T));
        };

        /// @brief Set up the default serialization traits for bool,
        /// which we'll stick in uint8_t (OSVR_CBool) types. Note that if you're
        /// going for VRPN compatibility (re-implementing existing VRPN
        /// messages), be sure you serialize vrpn_bool, not just bool - VPRN
        /// uses 16-bits for a bool by default IIRC.
        template <>
        struct SerializationTraits<DefaultSerializationTag<bool>, void>
            : BaseSerializationTraits<bool> {

            typedef BaseSerializationTraits<bool> Base;
            typedef DefaultSerializationTag<bool> tag_type;

            template <typename BufferType>
            static void serialize(BufferType &buf, Base::param_type val,
                                  tag_type const &) {
                serializeRaw(
                    buf, static_cast<OSVR_CBool>(val ? OSVR_TRUE : OSVR_FALSE));
            }
            template <typename BufferReaderType>
            static void deserialize(BufferReaderType &reader,
                                    Base::reference_type val,
                                    tag_type const &) {
                OSVR_CBool cVal;
                deserializeRaw(reader, cVal);
                val = (cVal == OSVR_TRUE);
            }
        };
        template <typename EnumType, typename IntegerType>
        struct SerializationTraits<EnumAsIntegerTag<EnumType, IntegerType>,
                                   void> : BaseSerializationTraits<EnumType> {
            typedef BaseSerializationTraits<EnumType> Base;
            typedef EnumAsIntegerTag<EnumType, IntegerType> tag_type;

            template <typename BufferType>
            static void serialize(BufferType &buf,
                                  typename Base::param_type val,
                                  tag_type const &) {
                serializeRaw(buf, static_cast<IntegerType>(val));
            }

            template <typename BufferReaderType>
            static void deserialize(BufferReaderType &reader,
                                    typename Base::reference_type val,
                                    tag_type const &) {
                IntegerType intVal;
                deserializeRaw(reader, intVal);
                val = static_cast<EnumType>(intVal);
            }
        };

        /// @brief String, length-prefixed. (default)
        template <>
        struct SerializationTraits<DefaultSerializationTag<std::string>, void>
            : BaseSerializationTraits<std::string> {

            typedef BaseSerializationTraits<std::string> Base;
            typedef DefaultSerializationTag<std::string> tag_type;

            typedef uint32_t length_type;

            template <typename BufferType>
            static void serialize(BufferType &buf, Base::param_type val,
                                  tag_type const &) {
                length_type len = val.length();
                serializeRaw(buf, len);
                buf.append(val.c_str(), len);
            }

            /// @brief Reads an object of this type from a buffer
            template <typename BufferReaderType>
            static void deserialize(BufferReaderType &reader,
                                    Base::reference_type val,
                                    tag_type const &) {
                length_type len;
                deserializeRaw(reader, len);
                auto iter = reader.readBytes(len);
                val.assign(iter, iter + len);
            }

            /// @brief Returns the number of bytes required for this type (and
            /// alignment padding if applicable) to be appended to a buffer of
            /// the supplied existing size.
            static size_t spaceRequired(size_t existingBytes,
                                        Base::param_type val,
                                        tag_type const &) {
                return getBufferSpaceRequiredRaw(existingBytes, length_type()) +
                       val.length();
            }
        };

        /// @brief Traits for the StringOnlyMessageTag: a string, not prefixed
        /// by length (only useful if a whole message is a string).
        template <>
        struct SerializationTraits<StringOnlyMessageTag, void>
            : BaseSerializationTraits<std::string> {

            typedef BaseSerializationTraits<std::string> Base;
            typedef StringOnlyMessageTag tag_type;

            template <typename BufferType>
            static void serialize(BufferType &buf, Base::param_type val,
                                  tag_type const &) {
                buf.append(val.c_str(), val.length());
            }

            template <typename BufferReaderType>
            static void deserialize(BufferReaderType &reader,
                                    Base::reference_type val,
                                    tag_type const &) {
                auto len = reader.bytesRemaining();
                auto iter = reader.readBytes(len);
                val.assign(iter, iter + len);
            }

            /// @brief Returns the number of bytes required for this type (and
            /// alignment padding if applicable) to be appended to a buffer of
            /// the supplied existing size.
            static size_t spaceRequired(size_t, Base::param_type val,
                                        tag_type const &) {
                return val.length();
            }
        };

        /// @brief Serialization traits for a raw data bytestream with the given
        /// alignment.
        template <> struct SerializationTraits<AlignedDataBufferTag, void> {
            typedef AlignedDataBufferTag tag_type;

            template <typename BufferType, typename DataType>
            static void serialize(BufferType &buf, DataType const *val,
                                  tag_type const &tag) {
                auto dataPtr =
                    reinterpret_cast<typename BufferType::ElementType const *>(
                        val);
                buf.appendAligned(dataPtr, tag.length(), tag.alignment());
            }

            template <typename BufferReaderType, typename DataType>
            static void deserialize(BufferReaderType &reader, DataType *val,
                                    tag_type const &tag) {
                auto len = tag.length();
                auto iter = reader.readBytesAligned(len, tag.alignment());
                std::copy(iter, iter + len, val);
            }

            /// @brief Returns the number of bytes required for this type (and
            /// alignment padding if applicable) to be appended to a buffer of
            /// the supplied existing size.
            template <typename DataType>
            static size_t spaceRequired(size_t existingBytes, DataType *val,
                                        tag_type const &tag) {
                return computeAlignmentPadding(tag.alignment(), existingBytes) +
                       tag.length();
            }
        };
    } // namespace serialization

} // namespace common
} // namespace osvr
#endif // INCLUDED_SerializationTraits_h_GUID_DF0CDFE0_097F_41B2_2DAE_7D4033D28D43
