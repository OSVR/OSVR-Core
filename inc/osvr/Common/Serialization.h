/** @file
    @brief Header

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

#ifndef INCLUDED_Serialization_h_GUID_339B4851_CA90_41A6_E1CC_4F94A18614AC
#define INCLUDED_Serialization_h_GUID_339B4851_CA90_41A6_E1CC_4F94A18614AC

// Internal Includes
#include <osvr/Common/Buffer.h>
#include <osvr/Common/Endianness.h>
#include <osvr/Util/StdInt.h>

// Library/third-party includes
#include <boost/call_traits.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>

namespace osvr {
namespace common {

    namespace serialization {
        template <typename T, typename BufferType>
        inline typename boost::enable_if<boost::is_integral<T> >::type
        serialize(BufferType &buf, T const val) {
            /// integer - handle endianness.
            bufferAppend(buf, hton(val));
        }
        template <typename T, typename BufferType>
        inline typename boost::enable_if<boost::is_floating_point<T> >::type
        serialize(BufferType &buf, T const val) {
            /// float - handle endianness.
            bufferAppend(buf, hton(val));
        }

        template <typename T, typename BufferType>
        inline typename boost::enable_if<boost::is_integral<T> >::type
        deserialize(T &val, BufferType const &buf,
                    typename BufferType::const_iterator &it) {
            /// Integer - handle endianness.
        }

        /// @brief The default "type tag" for specifying serialization behavior.
        ///
        /// Usage of tag types and matching type traits allows us to serialize
        /// the same underlying type different ways when requested.
        template <typename T> struct DefaultSerializationTag {
            typedef T type;
        };
        /// @brief Traits class indicating how to serialize a type with a given
        /// tag. The default tag for a type `T` is `DefaultSerializationTag<T>`
        ///
        /// The dummy template parameter exists for usage of `enable_if`.
        template <typename Tag, typename Dummy = void>
        struct SerializationTraits;

        template <typename T, size_t Alignment>
        struct DefaultSerializationTraits {
            BOOST_STATIC_ASSERT(Alignment > 1);
            static size_t spaceRequired(size_t existingBufferLength) {
                auto leftover = existingBufferLength % Alignment;
                return (leftover == 0) ? sizeof(T)
                                       : (Alignment - leftover) + sizeof(T);
            }
        };
        template <typename T>
        struct SerializationTraits<
            DefaultSerializationTag<T>,
            typename boost::enable_if<boost::is_integral<T> >::type> {};
        class SerializerFunctor : boost::noncopyable {
          public:
            /// @brief Constructor, taking in the destination buffer.
            SerializerFunctor(Buffer &buf) : m_buf(buf) {}

            /// @brief Main function call operator method, optionally taking a
            /// "tag type" to specify serialization-related behavior.
            ///
            /// @param v The value to process - in this case, to add to the
            /// buffer.
            template <typename T,
                      typename Tag = DefaultSerializationTypeTag<T> >
            void operator()(typename boost::call_traits<T>::param_type v,
                            Tag const & = Tag()) {}

          private:
            Buffer &m_buf;
        };

    } // namespace serialization
    /// @brief Template class, using the CRTP to produce message
    /// serialization/deserialization code. Your class should inherit from this
    /// class (with your class as the template parameter), and implement a
    /// method `template<typename T> void processMessage(T & process)` that
    /// calls
    template <typename Base> class MessageSerializationBase {
      public:
        /// @brief
        void serialize(Buffer &buf) {
            serialization::SerializerFunctor functor(buf);
        }
    };
} // namespace common
} // namespace osvr

#endif // INCLUDED_Serialization_h_GUID_339B4851_CA90_41A6_E1CC_4F94A18614AC
