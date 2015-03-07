/** @file
    @brief Header to bring unique_ptr into the ::osvr namespace.

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

#ifndef INCLUDED_UniquePtr_h_GUID_3CF3C525_BAAB_41B2_D659_A4D166EABB52
#define INCLUDED_UniquePtr_h_GUID_3CF3C525_BAAB_41B2_D659_A4D166EABB52

#if defined(_MSC_VER) && (_MSC_VER < 1600)
#error "Not supported before VS 2010"

#else
#include <memory>

namespace osvr {
using std::unique_ptr;
} // namespace osvr

#endif

#endif // INCLUDED_UniquePtr_h_GUID_3CF3C525_BAAB_41B2_D659_A4D166EABB52
