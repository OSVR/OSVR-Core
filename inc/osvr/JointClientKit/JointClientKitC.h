/** @file
    @brief Header

    Must be c-safe!

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

/*
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
*/

#ifndef INCLUDED_JointClientKitC_h_GUID_AEF51E42_05FE_4095_DC94_0D1E5CF76A76
#define INCLUDED_JointClientKitC_h_GUID_AEF51E42_05FE_4095_DC94_0D1E5CF76A76

/* Internal Includes */
#include <osvr/JointClientKit/Export.h>
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

/** @defgroup JointClientKit
    @brief In rare cases, it may be useful to run the server and client in a
    single process, with their mainloops sharing a single thread. This library
    exists to permit that.

    Note that because it transfers the overhead of having to perform
    configuration to the client application and reduces modularity, this API is
    not recommended except in very specialized situations. (Probably not your
    situations, that is - and if it turns out you need this, you can swap it in
    easily after writing your application the typical way.

    @{
*/

/** @brief Opaque typedef for options used when starting up a joint client
    context.
*/
typedef struct OSVR_JointClientContextOptsObject *OSVR_JointClientOpts;

/** @brief Creates an OSVR_JointClientOpts that specifies the full path to a
    JSON config file (as would be passed to the osvr_server app) to load on
    context init.

    @param configFilename (Preferably full) path to a JSON config file.

    @returns the options object - if NULL, an error occurred.
*/
OSVR_JOINTCLIENTKIT_EXPORT OSVR_JointClientOpts
osvrJointClientUseJsonConfig(const char *configFilename);

/** @brief Initialize the library, starting up a "joint" context that also
    contains a server.

    @param applicationIdentifier A null terminated string identifying your
    application. Reverse DNS format strongly suggested.
    @param opts The configuration options object for starting the joint server
    operations.

    @returns Client context - will be needed for subsequent calls
*/
OSVR_JOINTCLIENTKIT_EXPORT OSVR_ClientContext osvrJointClientInit(
    const char applicationIdentifier[], OSVR_JointClientOpts opts);

/** @} */
OSVR_EXTERN_C_END

#endif
