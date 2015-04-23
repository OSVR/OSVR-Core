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

#ifndef INCLUDED_IndentingStream_h_GUID_05296193_1397_4991_ACE4_D6FA08655E0B
#define INCLUDED_IndentingStream_h_GUID_05296193_1397_4991_ACE4_D6FA08655E0B

// Internal Includes
#include <osvr/Util/Flag.h>

// Library/third-party includes
#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/filtering_stream.hpp>

// Standard includes
#include <stddef.h>
#include <vector>
#include <iosfwd>

namespace osvr {
namespace util {
    namespace detail {
        /// @brief A boost::iostreams::output_filter that inserts the given
        /// number of spaces before the first character after a newline. Removes
        /// carriage returns as a side effect.
        class IndentFilter : public boost::iostreams::output_filter {
          public:
            IndentFilter(size_t spaces)
                : m_theSpaces(spaces, ' '), m_lastWasNewline(true) {}

            static const char NEWLINE = '\n';
            static const char CR = '\r';
            void setLastWasNewline() { m_lastWasNewline = true; }
            template <typename Sink> bool put(Sink &snk, char c) {

                namespace io = boost::iostreams;
                osvr::util::Flag failure;
                if (c == CR) {
                    // silently drop these, they can confuse us
                    return true;
                }
                if (c == NEWLINE) {
                    m_lastWasNewline = true;
                } else if (m_lastWasNewline) {
                    failure +=
                        !io::write(snk, m_theSpaces.data(), m_theSpaces.size());
                    m_lastWasNewline = false;
                }
                failure += !io::put(snk, c);
                return !failure;
            }

          private:
            std::vector<char> m_theSpaces;
            bool m_lastWasNewline;
        };
    } // namespace detail

    /// @brief A boost::iostreams::filtering_ostream with a constructor that
    /// automatically sets it up to indent the first character after a newline
    /// the given number of spaces and forward the output to a std::ostream.
    class IndentingStream : public boost::iostreams::filtering_ostream {
      public:
        IndentingStream(size_t spaces, std::ostream &stream)
            : m_filter(spaces), m_os(stream) {
            push(boost::ref(m_filter));
            push(m_os);
        }

        void reset() { m_filter.setLastWasNewline(); }

      private:
        detail::IndentFilter m_filter;
        std::ostream &m_os;
    };
} // namespace util
} // namespace osvr

#endif // INCLUDED_IndentingStream_h_GUID_05296193_1397_4991_ACE4_D6FA08655E0B
