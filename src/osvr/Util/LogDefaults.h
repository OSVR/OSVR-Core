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

#ifndef INCLUDED_LogDefaults_h_GUID_A18EEB05_68C0_45AA_C97B_F7D6CEEA1330
#define INCLUDED_LogDefaults_h_GUID_A18EEB05_68C0_45AA_C97B_F7D6CEEA1330

// Internal Includes
#include <osvr/Util/LogLevel.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace util {
    namespace log {
        static const auto DEFAULT_PATTERN = "%b %d %T.%e %l [%n]: %v";
        static const auto DEFAULT_LEVEL = LogLevel::trace;
        static const auto DEFAULT_CONSOLE_LEVEL = LogLevel::info;
        static const auto DEFAULT_FLUSH_LEVEL = LogLevel::info;
    } // end namespace log
} // end namespace util
} // end namespace osvr

#endif // INCLUDED_LogDefaults_h_GUID_A18EEB05_68C0_45AA_C97B_F7D6CEEA1330
