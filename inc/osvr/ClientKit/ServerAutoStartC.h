/** @file
    @brief API to attempt OSVR server auto-start.

    Must be c-safe!

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

/*
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
*/

#ifndef INCLUDED_ServerAutoStartC_h_GUID_126F2ADB_CC3E_4F99_86FA_0A58DC9490F6
#define INCLUDED_ServerAutoStartC_h_GUID_126F2ADB_CC3E_4F99_86FA_0A58DC9490F6

/* Internal Includes */
#include <osvr/ClientKit/Export.h>
#include <osvr/Util/APIBaseC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

/** @addtogroup ClientKit
@{
*/

/** @brief Ensures the OSVR server process/thread is running. Call once per process. The server may
or may not actually start successfully as a result of this call. Continue to check client
context status to determine if the client context has connected successfully.
*/
OSVR_CLIENTKIT_EXPORT void osvrClientAttemptServerAutoStart();

/** @brief Ensures any OSVR server-related resources in the client are released. Call once per process.
*/
OSVR_CLIENTKIT_EXPORT void osvrClientReleaseAutoStartedServer();

/** @} */
OSVR_EXTERN_C_END

#endif
