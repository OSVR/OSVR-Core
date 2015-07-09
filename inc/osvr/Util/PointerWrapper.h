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

#ifndef INCLUDED_PointerWrapper_h_GUID_0C8888FE_163F_46A3_A9F2_1EFCF610C64D
#define INCLUDED_PointerWrapper_h_GUID_0C8888FE_163F_46A3_A9F2_1EFCF610C64D

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace util {
    /// @brief A wrapper to easily define structures that exist just to hold a
    /// pointer.
    template <typename Contained> class PointerWrapper {
      public:
        PointerWrapper(Contained *ptr = nullptr) : m_ptr(ptr) {}

        operator Contained **() { return getContainerLocation(); }

        Contained **getContainerLocation() { return &m_ptr; }
        Contained *operator->() { return m_ptr; }
        Contained &operator*() { return *m_ptr; }

      private:
        Contained *m_ptr;
    };
} // namespace util
} // namespace osvr

#endif // INCLUDED_PointerWrapper_h_GUID_0C8888FE_163F_46A3_A9F2_1EFCF610C64D
