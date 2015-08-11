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

#ifndef INCLUDED_ID_h_GUID_137CA336_382A_4796_7735_4521F02D5AC2
#define INCLUDED_ID_h_GUID_137CA336_382A_4796_7735_4521F02D5AC2

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <string>
#include <stdint.h>

/// idea from the
/// http://www.ilikebigbits.com/blog/2014/5/6/type-safe-identifiers-in-c
template <class Tag, class impl> class ID {
  public:
    static ID invalid() { return ID(); }

    // Default constructor which will set m_val to a 0xffffffff (UINT32 max)
    // and signify empty.
    ID() : m_val(0xffffffff) {}

    // Explicit constructor:
    explicit ID(impl val) : m_val(val) {}

    // Explicit conversion to get back the impl:
    // explicit operator impl() const { return m_val; }

    // Implicit conversion to get back the impl
    operator impl() const { return m_val; }

    bool empty() const { return m_val == 0xffffffff ? true : false; }

    // this messes with implicit conversion (type casting)
    /*
    friend bool operator>(ID a, impl b) { return a.m_val > b; }
    friend bool operator<(ID a, impl b) { return a.m_val < b; }
    friend bool operator==(ID a, ID b) { return a.m_val == b.m_val; }
    friend bool operator!=(ID a, ID b) { return a.m_val != b.m_val; }
    */
    impl m_val;
};

typedef ID<struct StringTag, uint32_t> StringID;
typedef ID<struct StringTag, uint32_t> PeerStringID;

#endif // INCLUDED_ID_h_GUID_137CA336_382A_4796_7735_4521F02D5AC2
