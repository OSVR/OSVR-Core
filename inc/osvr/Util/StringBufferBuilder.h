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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_StringBufferBuilder_h_GUID_D531C5F4_AC8D_4E9D_9682_A38DCBC7DC8C
#define INCLUDED_StringBufferBuilder_h_GUID_D531C5F4_AC8D_4E9D_9682_A38DCBC7DC8C

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <vector>
#include <string>
#include <cstddef>

namespace osvr {
namespace util {
    /// @brief A utility class to adapt APIs that first provide a length, then
    /// place a string in a user-allocated buffer, to nearly transparently
    /// handle the buffer and convert into a std::string.
    class StringBufferBuilder {
      public:
        /// @brief Allocates a buffer of size n, to include the null terminator,
        /// and returns a pointer into it.
        char *getBufferOfSize(std::size_t n) {
            /// Allocate one more character for safety, in case the user's size
            /// didn't include the null terminator.
            m_buffer.resize(n + 1, '\0');
            return m_buffer.data();
        }
        /// @brief Converts the buffer into a string.
        ///
        /// Not const to be able to trim trailing nulls without making a
        /// copy or doing things with iterators that felt dangerous. Since
        /// this is a class for short-term use, that should be fine.
        std::string str() {
            /// Create this right away for the NRVO optimization.
            std::string ret;
            if (m_buffer.empty()) {
                return ret;
            }
            /// Drop trailing nulls - the string constructor doesn't take nulls
            /// unless you just hand it a cstr, may as well use the fact we know
            /// length.
            while (!m_buffer.empty() && m_buffer[m_buffer.size() - 1] == '\0') {
                m_buffer.pop_back();
            }

            if (m_buffer.empty()) {
                return ret;
            }
            ret.assign(m_buffer.begin(), m_buffer.end());
            return ret;
        }

      private:
        typedef std::vector<char> container;
        container m_buffer;
    };
}
}
#endif // INCLUDED_StringBufferBuilder_h_GUID_D531C5F4_AC8D_4E9D_9682_A38DCBC7DC8C
