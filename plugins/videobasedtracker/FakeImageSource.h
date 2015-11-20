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

#ifndef INCLUDED_FakeImageSource_h_GUID_BE629DEC_1342_4280_1316_24D5A4A5A239
#define INCLUDED_FakeImageSource_h_GUID_BE629DEC_1342_4280_1316_24D5A4A5A239

// Internal Includes
#include "ImageSource.h"

// Library/third-party includes
// - none

// Standard includes
#include <string>

namespace osvr {
namespace vbtracker {
    ImageSourcePtr openImageFileSequence(std::string const &dir);
} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_FakeImageSource_h_GUID_BE629DEC_1342_4280_1316_24D5A4A5A239
