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
#include <osvr/Common/BufferTraits.h>

// Library/third-party includes
#include <boost/call_traits.hpp>
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>

namespace osvr {
namespace common {

    namespace serialization {

        /// @brief Functor class used by osvr::common::serialize to serialize a
        /// message (passed as the "process" argument to the class's
        /// processMessage method).
        template <typename BufferType>
        class SerializeFunctor : boost::noncopyable {
          public:
            /// @brief Constructor, taking the buffer
            SerializeFunctor(BufferType &buf) : m_buf(buf) {}

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
            /// @brief Implementation of action, using call_traits for optimized
            /// value handling.
            template <typename T, typename Tag>
            void apply(typename boost::call_traits<T>::param_type v,
                       Tag const &tag = Tag()) {
                serializeRaw(m_buf, v, tag);
            }
            BufferType &m_buf;
        };

        /// @brief Functor class used by osvr::common::deserialize to
        /// deserialize a message (passed as the "process" argument to the
        /// class's processMessage method).
        template <typename BufferReaderType>
        class DeserializeFunctor : boost::noncopyable {
          public:
            /// @brief Constructor, taking the buffer reader
            DeserializeFunctor(BufferReaderType &reader) : m_reader(reader) {}

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
                deserializeRaw(m_reader, v, tag);
            }
            BufferReaderType &m_reader;
        };

    } // namespace serialization

    /// @brief Serializes a message into a buffer, using a `MessageClass`
    ///
    /// Your `MessageClass` class must implement a method `template<typename T>
    /// void processMessage(T & process)` that calls `process()` with each field
    /// (optionally providing a tag type). After each such call, the fields
    /// processed so far are guaranteed to contain valid data (in this case, the
    /// same data they started with), in case your `processMessage()` method
    /// needs to perform computation.
    template <typename BufferType, typename MessageClass>
    void serialize(BufferType &buf, MessageClass &msg) {
        BOOST_STATIC_ASSERT(is_buffer<BufferType>::value);
        /// @todo add another functor to first compute message length and
        /// reserve buffer space?
        serialization::SerializeFunctor<BufferType> functor(buf);
        msg.processMessage(functor);
    }
    /// @brief Deserializes a message from a buffer, using a `MessageClass`
    ///
    /// Your `MessageClass` class must implement a method `template<typename T>
    /// void processMessage(T & process)` that calls `process()` with each field
    /// (optionally providing a tag type). After each such call, the fields
    /// processed so far are guaranteed to contain valid data (in this case, the
    /// data deserialized from the buffer), in case your `processMessage()`
    /// method needs to perform computation.
    template <typename BufferReaderType, typename MessageClass>
    void deserialize(BufferReaderType &reader, MessageClass &msg) {
        BOOST_STATIC_ASSERT(is_buffer_reader<BufferReaderType>::value);
        serialization::DeserializeFunctor<BufferReaderType> functor(reader);
        msg.processMessage(functor);
    }
} // namespace common
} // namespace osvr

#endif // INCLUDED_Serialization_h_GUID_339B4851_CA90_41A6_E1CC_4F94A18614AC
