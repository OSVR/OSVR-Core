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

#ifndef INCLUDED_Flag_h_GUID_9D7B0F8D_1156_4B8A_21D0_64729F213C14
#define INCLUDED_Flag_h_GUID_9D7B0F8D_1156_4B8A_21D0_64729F213C14

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace util {

    /// @brief A class that lightly wraps a bool, in order to provide easier
    /// maintenance of a "dirty" flag, for example, with sensible default
    /// initialization and no short-circuiting.
    class Flag {
      public:
        /// @brief Initialize the flag as false.
        Flag() : m_val(false) {}

        /// @brief Be able to test the flag in an if statement.
        explicit operator bool() const { return get(); }

        /// @brief Get the value of the flag as a bool.
        bool get() const { return m_val; }

        /// @brief Set the flag to true
        void set() { m_val = true; }

        /// @brief Update the flag with (typically) a function call result:
        /// state is true if state was true or newVal is true
        Flag &operator+=(bool newVal) {
            m_val |= newVal;
            return *this;
        }
        /// @brief Update the flag with another flag.
        Flag &operator+=(Flag const &rhs) {
            m_val |= rhs.m_val;
            return *this;
        }

        /// @brief Reset the flag back to false
        void reset() { m_val = false; }

      private:
        bool m_val;
    };

} // namespace util
} // namespace osvr
#endif // INCLUDED_Flag_h_GUID_9D7B0F8D_1156_4B8A_21D0_64729F213C14
