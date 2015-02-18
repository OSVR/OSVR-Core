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
        /// @brief Serialize a value to a buffer, with optional tag to specify
        /// non-default traits.
        template <typename T, typename BufferType,
                  typename Tag = DefaultSerializationTag<T> >
        inline void serializeRaw(BufferType &buf, T const &v,
                                 Tag const &tag = Tag()) {
            SerializationTraits<Tag>::buffer(buf, v, tag);
        }

        /// @brief Deserialize a value from a buffer, with optional tag to
        /// specify non-default traits.
        template <typename T, typename BufferReaderType,
                  typename Tag = DefaultSerializationTag<T> >
        inline void deserializeRaw(BufferReaderType &reader, T &v,
                                   Tag const &tag = Tag()) {
            SerializationTraits<Tag>::unbuffer(reader, v, tag);
        }

        /// @brief Functor class used by MessageSerializationBase to serialize a
        /// message (passed as the "process" argument to the derived class's
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

        /// @brief Functor class used by MessageSerializationBase to deserialize
        /// a message (passed as the "process" argument to the derived class's
        /// processMessage method).
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
    /// @brief Template class, using the CRTP to produce message
    /// serialization/deserialization code. Your class should inherit from this
    /// class (with your class as the template parameter), and implement a
    /// method `template<typename T> void processMessage(T & process)` that
    /// calls the function call operator with each member and an optional tag
    /// type.
    template <typename Base> class MessageSerializationBase {
      public:
        /// @brief Serialize to buffer.
        template <typename BufferType> void serialize(BufferType &buf) {
            serialization::SerializeFunctor<BufferType> functor(buf);
            static_cast<Base *>(this)->processMessage(functor);
        }
    };

    /// @brief Serializes a message into a buffer, using a `MessageClass`
    ///
    /// Your `MessageClass` class must implement a method `template<typename T>
    /// void processMessage(T & process)` that calls `process()` with each field
    /// (optionally providing a tag type). After each such call, the fields
    /// processed so far are guaranteed to contain valid data (in this case, the
    /// same data they started with), in case your `processMessage()` method
    /// needs to perform computation.
    template <typename MessageClass, typename BufferType>
    void serialize(BufferType &buf, MessageClass &msg) {
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
    template <typename MessageClass, typename BufferReaderType>
    void deserialize(BufferReaderType &reader, MessageClass &msg) {
        serialization::DeserializeFunctor<BufferReaderType> functor(reader);
        msg.processMessage(functor);
    }
} // namespace common
} // namespace osvr

#endif // INCLUDED_Serialization_h_GUID_339B4851_CA90_41A6_E1CC_4F94A18614AC
