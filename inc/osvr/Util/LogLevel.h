/** @file
    @brief Header

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com>

*/

// Copyright 2016 Sensics, Inc.
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

#ifndef INCLUDED_LogLevel_h_GUID_721FFB5D_B4A7_416A_B11E_982C1A21795B
#define INCLUDED_LogLevel_h_GUID_721FFB5D_B4A7_416A_B11E_982C1A21795B

// Internal Includes
#include <osvr/Util/LogLevelC.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace util {
namespace log {

    /**
     * @brief Log message severity levels.
     */
    enum class LogLevel {
        trace    = OSVR_LOGLEVEL_TRACE,    //< function entry and exit, control flow.
        debug    = OSVR_LOGLEVEL_DEBUG,    //< developer-facing messages.
        info     = OSVR_LOGLEVEL_INFO,     //< user-facing messages.
        notice   = OSVR_LOGLEVEL_NOTICE,   //< normal but significant condition.
        warn     = OSVR_LOGLEVEL_WARN,     //< warning conditions.
        err      = OSVR_LOGLEVEL_ERR,      //< error messages.
        critical = OSVR_LOGLEVEL_CRITICAL, //< critical conditions.
        alert    = OSVR_LOGLEVEL_ALERT,    //< action must be taken immediately.
        emerg    = OSVR_LOGLEVEL_EMERG,    //< system is unusable.
    };

} // end namespace log
} // end namespace util
} // end namespace osvr

#endif // INCLUDED_LogLevel_h_GUID_721FFB5D_B4A7_416A_B11E_982C1A21795B

