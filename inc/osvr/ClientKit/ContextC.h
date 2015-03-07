/** @file
    @brief Header

    Must be c-safe!

    @todo Apply annotation macros

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

/*
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
*/

#ifndef INCLUDED_ContextC_h_GUID_3790F330_2425_4486_4C9F_20C300D7DED3
#define INCLUDED_ContextC_h_GUID_3790F330_2425_4486_4C9F_20C300D7DED3

/* Internal Includes */
#include <osvr/ClientKit/Export.h>
#include <osvr/Util/APIBaseC.h>
#include <osvr/Util/ReturnCodesC.h>
#include <osvr/Util/AnnotationMacrosC.h>
#include <osvr/Util/StdInt.h>
#include <osvr/Util/ClientOpaqueTypesC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

/** @addtogroup ClientKit
    @{
*/

/** @brief Initialize the library.

    @param applicationIdentifier A null terminated string identifying your
   application. Reverse DNS format strongly suggested.
    @param flags initialization options (reserved) - pass 0 for now.

    @returns Client context - will be needed for subsequent calls
*/
OSVR_CLIENTKIT_EXPORT OSVR_ClientContext
osvrClientInit(const char applicationIdentifier[],
               uint32_t flags OSVR_CPP_ONLY(= 0));

/** @brief Updates the state of the context - call regularly in your mainloop.

    @param ctx Client context
*/
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode osvrClientUpdate(OSVR_ClientContext ctx);

/** @brief Shutdown the library.
    @param ctx Client context
*/
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode
osvrClientShutdown(OSVR_ClientContext ctx);

/** @} */
OSVR_EXTERN_C_END

#endif
