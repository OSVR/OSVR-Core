/** @file
    @brief Platform agnostic process utilities.

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

#ifndef INCLUDED_ProcessUtils_h_GUID_6C3F7688_AF0D_4094_D629_AF8DBFDF2044
#define INCLUDED_ProcessUtils_h_GUID_6C3F7688_AF0D_4094_D629_AF8DBFDF2044


// Internal Includes
#include <osvr/Util/APIBaseC.h>
#include <osvr/Util/ReturnCodesC.h>
#include <osvr/Util/PlatformConfig.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#if !defined(OSVR_ANDROID)
#if defined(OSVR_WINDOWS)
#include <windows.h>
#else
#include <unistd.h>
#endif
#endif

// Standard includes
// - none

/** @brief INTERNAL ONLY - start a process in a platform agnostic way */
OSVR_INLINE OSVR_ReturnCode osvrStartProcess(const char* executablePath, const char* workingDirectory)
{
#if defined(OSVR_ANDROID)
    // @todo: can we just use the *nix implementation for android?
    OSVR_DEV_VERBOSE("osvrStartProces not yet implemented for Android");
    return OSVR_RETURN_FAILURE;
#elif defined(OSVR_WINDOWS)
    STARTUPINFO startupInfo = { 0 };
    PROCESS_INFORMATION processInfo = { 0 };

    startupInfo.dwFlags |= STARTF_USESHOWWINDOW;
    startupInfo.wShowWindow = SW_SHOW;
    if (!CreateProcess(executablePath, nullptr, nullptr, nullptr, FALSE, CREATE_NEW_CONSOLE, nullptr,
        workingDirectory, &startupInfo, &processInfo)) {
        OSVR_DEV_VERBOSE("Could not start process.");
        return OSVR_RETURN_FAILURE;
    }
#else
    OSVR_DEV_VERBOSE("osvrStartProcess not yet implemented for non-Windows platforms");
    return OSVR_RETURN_FAILURE;
    // UNTESTED (or even compiled)
    //pid_t pid = fork();
    //if (pid == -1) {
    //    OSVR_DEV_VERBOSE("Could not fork the process.");
    //    return OSVR_RETURN_FAILURE;
    //} else if (pid == 0) {
    //    // @todo set the current working directory to workingDirectory here
    //    execl(executablePath, NULL);
    //    OSVR_DEV_VERBOSE("Could not execute the process.");
    //    return OSVR_RETURN_FAILURE;
    //} else {
    //    // @todo: is this too verbose?
    //    //OSVR_DEV_VERBOSE("Started OSVR server process successfully.");
    //    return OSVR_RETURN_SUCCESS;
    //}
#endif
    return OSVR_RETURN_SUCCESS;
}

#endif // INCLUDED_ProcessUtils_h_GUID_6C3F7688_AF0D_4094_D629_AF8DBFDF2044

