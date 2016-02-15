/** @file
    @brief Header

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_CheckDeprecatedParameter_h_GUID_DCDA2F76_E0E5_449C_39A9_2BC6ABB4C405
#define INCLUDED_CheckDeprecatedParameter_h_GUID_DCDA2F76_E0E5_449C_39A9_2BC6ABB4C405

// Internal Includes
// - none

// Library/third-party includes
#include <json/value.h>

// Standard includes
#include <sstream>

namespace osvr {
namespace vbtracker {
    namespace detail {
        /// Dummy type to be used to indicate messages should be discarded
        /// rather than stored/streamed.
        struct discard_message_t {};
        /// Pass as a constructor argument to indicate that messages should be
        /// discarded.
        extern const discard_message_t discard_message;
        /// Flexible message streaming class that can either accumulate messages
        /// in a stream or ignore them completely.
        ///
        /// Inspired by the Message class in GTest but with extra C++11 juice.
        class Message {
          public:
            /// Constructs an empty message with a stream. Pass
            /// `discard_message` if you don't want a stream.
            Message() : m_ss(new std::ostringstream) {}

            /// Constructor called with `discard_message` - will not save any
            /// data streamed to it.
            explicit Message(discard_message_t const &) {}

            /// Construct a message with an internal stream and initial
            /// contents.
            explicit Message(const char *initialMessage) : Message() {
                (*m_ss) << initialMessage;
            }

            Message(Message const &) = delete;

            Message(Message &&other) : m_ss(std::move(other.m_ss)) {}
            Message &operator=(Message const &) = delete;

            /// Checks to see if this message has an internal stream.
            bool hasStream() const { return static_cast<bool>(m_ss); }
            explicit operator bool() const { return hasStream(); }

            /// Stream me anything! (I might do nothing with it, but...)
            template <typename T> Message &operator<<(T &&rhs) {
#ifdef _MSC_VER
                using ::operator<<;
#endif
                if (hasStream()) {
                    (*m_ss) << std::forward<T>(rhs);
                }
                return *this;
            }

            /// Stream me to an ostream! (I might not have anything, but...)
            friend inline std::ostream &operator<<(std::ostream &os,
                                                   Message const &msg) {
                if (msg) {
                    os << (*msg.m_ss).str();
                }
                return os;
            }

          private:
            std::unique_ptr<std::ostringstream> m_ss;
        };

        /// Message stream wrapper like above, except this one can dump its
        /// contents to an ostream when it's destructed (like the end of an
        /// expression)
        class MessageStreamsAfterExpression {
          public:
            /// Nothing to stream to on destruction, so internal stream is set
            /// to discard
            MessageStreamsAfterExpression() : m_msg(discard_message) {}

            /// Construct with a stream for use at destruction and thus setting
            /// up internal stream as well.
            explicit MessageStreamsAfterExpression(std::ostream &os)
                : m_msg(), m_os(&os) {}

            /// Construct a message with an internal stream and initial
            /// contents.
            MessageStreamsAfterExpression(std::ostream &os,
                                          const char *initialMessage)
                : m_msg(initialMessage), m_os(&os) {}

            MessageStreamsAfterExpression(
                MessageStreamsAfterExpression const &) = delete;

            MessageStreamsAfterExpression
            operator=(MessageStreamsAfterExpression const &) = delete;

            MessageStreamsAfterExpression(MessageStreamsAfterExpression &&other)
                : m_msg(std::move(other.m_msg)), m_os(other.m_os) {
                /// clear the moved-from stream pointer so it doesn't try to
                /// print on its destruction.
                other.m_os = nullptr;
            }

            ~MessageStreamsAfterExpression() {
                if (m_os && m_msg) {
                    (*m_os) << m_msg << std::endl;
                }
            }

            /// Stream me anything! (I might do nothing with it, but...)
            template <typename T>
            MessageStreamsAfterExpression &operator<<(T &&rhs) {
                if (m_msg) {
                    // checking first to save a forward in no-op cases, which
                    // are expected to be most common.
                    m_msg << std::forward<T>(rhs);
                }
                return *this;
            }

          private:
            Message m_msg;
            std::ostream *m_os = nullptr;
        };
    } // namespace detail

    inline detail::MessageStreamsAfterExpression
    checkDeprecatedParameter(std::ostream &os, Json::Value const &obj,
                             const char *key) {
        Json::Value const &node = obj[key];
        if (node.isNull()) {
            return detail::MessageStreamsAfterExpression{};
        }
        return std::move(detail::MessageStreamsAfterExpression{os});
    }
} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_CheckDeprecatedParameter_h_GUID_DCDA2F76_E0E5_449C_39A9_2BC6ABB4C405
