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
#include <osvr/Common/SerializationTraits.h>

// Library/third-party includes
#include <boost/call_traits.hpp>
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>

namespace osvr {
namespace common {

    namespace serialization {
        /// @brief Functor class used by MessageSerializationBase to serialize a
        /// message (passed as the "process" argument to the derived class's
        /// processMessage method).
        template <typename BufferWrapperType>
        class SerializeFunctor : boost::noncopyable {
          public:
            /// @brief Constructor, taking the buffer
            SerializeFunctor(BufferWrapperType &buf) : m_buf(buf) {}

            /// @brief Implementation of action, using call_traits for optimized
            /// value handling.
            template <typename T, typename Tag>
            void apply(typename boost::call_traits<T>::param_type v,
                       Tag const &tag = Tag()) {
                SerializationTraits<Tag>::buffer(m_buf, v, tag);
            }

            /// @brief Main function call operator method.
            ///
            /// Thin wrapper around apply() to perform template argument
            /// deduction.
            ///
            /// @param v The value to process - in this case, to add to the
            /// buffer.
            template <typename T> void operator()(T const &v) {
                apply<T, DefaultSerializationTag<T> >(v);
            }

            /// @brief Main function call operator method, taking a "tag type"
            /// to specify non-default serialization-related behavior.
            ///
            /// Thin wrapper around apply() to perform template argument
            /// deduction.
            ///
            /// @param v The value to process - in this case, to add to the
            /// buffer.
            template <typename Tag, typename T>
            void operator()(T const &v, Tag const &tag = Tag()) {
                apply<T, Tag>(v, tag);
            }

          private:
            BufferWrapperType &m_buf;
        };

        /// @brief Functor class used by MessageSerializationBase to deserialize
        /// a message (passed as the "process" argument to the derived class's
        /// processMessage method).
        template <typename BufferReaderType>
        class DeserializeFunctor : boost::noncopyable {
          public:
#if 0
            typedef BufferWrapper<>::Reader Reader;
#endif
            typedef BufferReaderType Reader;
            /// @brief Constructor, taking the buffer reader
            DeserializeFunctor(Reader &reader) : m_reader(reader) {}

            /// @brief Main function call operator method.
            ///
            /// @param v The value to process - in this case, to read from the
            /// buffer.
            template <typename T> void operator()(T &v) {
                apply<T, DefaultSerializationTag<T> >(v);
            }

            /// @brief Main function call operator method, taking a "tag type"
            /// to specify non-default serialization-related behavior.
            ///
            /// @param v The value to process - in this case, to read from the
            /// buffer.
            template <typename Tag, typename T>
            void operator()(T &v, Tag const &tag = Tag()) {

                apply<T, Tag>(v, tag);
            }

          private:
            /// @brief Implementation of action, using call_traits for optimized
            /// value handling.
            template <typename T, typename Tag>
            void apply(typename boost::call_traits<T>::reference v,
                       Tag const &tag = Tag()) {
                SerializationTraits<Tag>::unbuffer(m_reader, v, tag);
            }
            Reader &m_reader;
        };

    } // namespace serialization
    /// @brief Template class, using the CRTP to produce message
    /// serialization/deserialization code. Your class should inherit from this
    /// class (with your class as the template parameter), and implement a
    /// method `template<typename T> void processMessage(T & process)` that
    /// calls the function call operator with each member and an optional tag
    /// type.
    template <typename Base> class MessageSerializationBase {
      public:
        /// @brief Serialize to buffer.
        template <typename BufferWrapperType>
        void serialize(BufferWrapperType &buf) {
            serialization::SerializeFunctor<BufferWrapperType> functor(buf);
            static_cast<Base *>(this)->processMessage(functor);
        }
    };
} // namespace common
} // namespace osvr

#endif // INCLUDED_Serialization_h_GUID_339B4851_CA90_41A6_E1CC_4F94A18614AC
