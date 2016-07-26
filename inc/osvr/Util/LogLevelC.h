/** @file
    @brief Header

    Must be c-safe!

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

#ifndef INCLUDED_LogLevelC_h_GUID_FF2DBB1E_A452_44B9_AE54_491C87D061D6
#define INCLUDED_LogLevelC_h_GUID_FF2DBB1E_A452_44B9_AE54_491C87D061D6


// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
// - none

/**
 * @brief Log message severity levels.
 */
typedef enum OSVR_LogLevel {
    OSVR_LOGLEVEL_TRACE = 0,     //< function entry and exit, control flow.
    OSVR_LOGLEVEL_DEBUG = 10,    //< debug-level messages.
    OSVR_LOGLEVEL_INFO = 20,     //< informational messages.
    OSVR_LOGLEVEL_NOTICE = 30,   //< normal but significant condition.
    OSVR_LOGLEVEL_WARN = 40,     //< warning conditions.
    OSVR_LOGLEVEL_ERR = 50,      //< error messages.
    OSVR_LOGLEVEL_CRITICAL = 60, //< critical conditions.
} OSVR_LogLevel;

#endif // INCLUDED_LogLevelC_h_GUID_FF2DBB1E_A452_44B9_AE54_491C87D061D6

