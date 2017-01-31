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

#ifndef INCLUDED_RangeTransform_h_GUID_94F5FF2F_27AC_44A5_FE5A_F9294C3DCCEB
#define INCLUDED_RangeTransform_h_GUID_94F5FF2F_27AC_44A5_FE5A_F9294C3DCCEB

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <algorithm>
#include <iterator>
#include <utility>

namespace osvr {
namespace vbtracker {
    /// Wrapper for std::transform that does the begin and end business for
    /// us.
    template <typename Container, typename OutIter, typename Func>
    inline OutIter range_transform(Container &&c, OutIter dest, Func &&f) {
        using std::begin;
        using std::end;
        return std::transform(begin(std::forward<Container>(c)),
                              end(std::forward<Container>(c)), dest,
                              std::forward<Func>(f));
    }
} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_RangeTransform_h_GUID_94F5FF2F_27AC_44A5_FE5A_F9294C3DCCEB
