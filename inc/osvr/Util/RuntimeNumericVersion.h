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

#ifndef INCLUDED_RuntimeNumericVersion_h_GUID_2B8A8F86_5430_4CBD_36E1_987F3EC0AF66
#define INCLUDED_RuntimeNumericVersion_h_GUID_2B8A8F86_5430_4CBD_36E1_987F3EC0AF66

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <tuple>
#include <stdint.h>

namespace osvr {
namespace util {
    typedef uint16_t NumericVersionComponent;
    typedef std::tuple<NumericVersionComponent, NumericVersionComponent,
                       NumericVersionComponent,
                       NumericVersionComponent> RuntimeNumericVersion;
} // namespace util
} // namespace osvr

#endif // INCLUDED_RuntimeNumericVersion_h_GUID_2B8A8F86_5430_4CBD_36E1_987F3EC0AF66
