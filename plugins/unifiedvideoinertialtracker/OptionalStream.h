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

#ifndef INCLUDED_OptionalStream_h_GUID_2C133DE4_22D9_4195_B36E_0663DF0A6378
#define INCLUDED_OptionalStream_h_GUID_2C133DE4_22D9_4195_B36E_0663DF0A6378

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <iosfwd>

namespace osvr {
namespace vbtracker {
    namespace detail {

        class OptionalStream {
          public:
            /// Nothing to stream to
            OptionalStream() {}

            /// Construct with a stream for use
            explicit OptionalStream(std::ostream &os) : m_os(&os) {}

            OptionalStream(OptionalStream const &) = delete;

            OptionalStream operator=(OptionalStream const &) = delete;

            OptionalStream(OptionalStream &&other) : m_os(other.m_os) {
                /// clear the moved-from stream pointer
                other.m_os = nullptr;
            }

            ~OptionalStream() {
                /// If we had a stream, put an endl on it.
                if (m_os) {
                    (*m_os) << std::endl;
                }
            }

            /// Stream me anything! (I might do nothing with it, but...)
            /// @todo figure out how to deal with std::endl and friends, who are
            /// overloaded functions...
            template <typename T> OptionalStream &operator<<(T &&rhs) {
                if (m_os) {
                    // checking first to save a forward in no-op cases, which
                    // are expected to be most common.
                    (*m_os) << std::forward<T>(rhs);
                }
                return *this;
            }
            /// Stream me anything! (I might do nothing with it, but...)
            template <typename T> OptionalStream &operator<<(T const &rhs) {
                if (m_os) {
                    // checking first to save a forward in no-op cases, which
                    // are expected to be most common.
                    (*m_os) << rhs;
                }
                return *this;
            }

          private:
            std::ostream *m_os = nullptr;
        };
    } // namespace detail
    inline detail::OptionalStream outputIf(std::ostream &os, bool condition) {
        return condition ? detail::OptionalStream{os}
                         : detail::OptionalStream{};
    }
    inline detail::OptionalStream outputUnless(std::ostream &os,
                                               bool condition) {
        return !condition ? detail::OptionalStream{os}
                          : detail::OptionalStream{};
    }
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_OptionalStream_h_GUID_2C133DE4_22D9_4195_B36E_0663DF0A6378
