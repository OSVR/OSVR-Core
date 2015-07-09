/** @file
    @brief Header providing a template function to reset a range of smart
   pointers.

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
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

#ifndef INCLUDED_ResetPointerList_h_GUID_067DF9C9_67F7_4293_94EF_DBC5B5266801
#define INCLUDED_ResetPointerList_h_GUID_067DF9C9_67F7_4293_94EF_DBC5B5266801

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace util {

    /// @brief Reset every smart pointer in a container one by one
    template <typename RangeType>
    inline void resetPointerRange(RangeType range) {
        typedef typename RangeType::type IteratorType;
        typedef typename IteratorType::value_type PointerType;
        for (auto &ptr : range) {
            ptr.reset();
        }
    }
} // namespace util
} // namespace osvr

#endif // INCLUDED_ResetPointerList_h_GUID_067DF9C9_67F7_4293_94EF_DBC5B5266801
