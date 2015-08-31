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
    @brief A limited-purpose library for creating client contexts that operate
    the server in the same thread. **See cautions.**

    In rare cases, it may be useful to run the server and client in
    a **single process**, with their mainloops sharing a **single thread**.
    Examples include automated tests, special-purpose apps, or apps on platforms
    that do not support interprocess communication or multiple threads (in which
    case no async plugins can be used either). This library exists to permit
    those limited use cases.

    Use of this library may sound appealing for a number of other use cases, but
    consider carefully, as there's likely a better way to solve the problem. In
    particular, this is **not** a good solution for the following:

    - avoiding having to start the server before your app: there are
    platform-specific "launch on demand" service techniques that should be used
    instead. (If they're not yet used in OSVR on the platform you're interested
    in, file an issue or contribute!)
    - other desires for simplification: this actually makes the overall user
    experience more complex, since you're eliminating some of the advantages of
    OSVR (centralized configuration of hardware-agnostic interfaces used by
    applications), and taking the responsibility of properly configuring a
    user's system when they've likely already configured it externally.
    - seeking to reduce latency due to communication between threads or
    processes: using this library will almost certainly increase latency,
    and **will** decrease your framerate if you don't put it in its own
    thread (and handle locking, etc. yourself), as you'd then have all the
    device servicing code right in the "hot path" of your app's main loop,
    instead of running asynchronously at a much higher rate in a separate
    thread and process.

    Because it transfers the overhead of having to perform configuration to the
    client application, reduces modularity, and can reduce performance, this API
    is not recommended except in very specialized situations. (Probably not your
    situations, that is - and if it turns out that you do actually need this,
    you can swap it in easily after writing your application the typical way,
    since the API only differs in the startup period.)

   Now that we've covered the warnings...

   ## General usage

   The usage pattern is very similar to ClientKit, and most
   functions you call are actually in ClientKit. The only differences relate to
   configuring the server that will run jointly:

   - On startup, you'll usually create a ::OSVR_JointClientOpts with
   osvrJointClientCreateOptions(), instead of directly creating an
   ::OSVR_ClientContext.
   - Call the various @ref JointClientKit methods with the
   ::OSVR_JointClientOpts you created to queue up server configuration actions.
   - To get an ::OSVR_ClientContext, you'll call osvrJointClientInit() with
   your application identifier (as in @ref ClientKit), also passing in the
   OSVR_JointClientOpts. (That method takes ownership of the options object and
   deletes it when finished.)
   - osvrJointClientInit() will create a special client context containg both a
   client and a server, then execute the actions you queued in the
   OSVR_JointClientOpts. If all those complete without exception, you'll receive
   a OSVR_ClientContext that you can proceed to use as normal with all @ref
   ClientKit methods. If any errors occur, you'll receive a null value instead.

    @{
*/

/** @brief Opaque typedef for options used when starting up a joint client
    context. Serves as a queue for configuration operations to perform on the
   server.
*/
typedef struct OSVR_JointClientContextOptsObject *OSVR_JointClientOpts;

/** @brief Creates an empty OSVR_JointClientOpts.

    The only way to deallocate this object is to pass it to
    osvrJointClientInit().

    @returns the options object - if NULL, an error occurred.
*/
OSVR_JOINTCLIENTKIT_EXPORT OSVR_JointClientOpts osvrJointClientCreateOptions();

/** @name Server configuration actions
    @brief Methods that queue up actions to perform on the server when the joint
    client/server is later created in osvrJointClientInit(). These methods do
    not perform the actions directly or immediately, and only perform some basic
    error checking on their inputs: failure in the actions specified may instead
    be reported later in a failure to create the context.

    @{
*/
/** @brief Queues up the autoloading of plugins. May only be called once per
    options object.
*/
OSVR_JOINTCLIENTKIT_EXPORT OSVR_ReturnCode
osvrJointClientOptionsAutoloadPlugins(OSVR_JointClientOpts opts);

/** @brief Queues up the manual load of a plugin by name.
*/
OSVR_JOINTCLIENTKIT_EXPORT OSVR_ReturnCode osvrJointClientOptionsLoadPlugin(
    OSVR_JointClientOpts opts, const char *pluginName);

/** @brief Queues up the manual instantiation of a plugin/driver by name with
    optional parameters (JSON).
*/
OSVR_JOINTCLIENTKIT_EXPORT OSVR_ReturnCode
osvrJointClientOptionsInstantiateDriver(OSVR_JointClientOpts opts,
                                        const char *pluginName,
                                        const char *driverName,
                                        const char *params);

/** @brief Queues up the manual addition of an alias to the path tree.
*/
OSVR_JOINTCLIENTKIT_EXPORT OSVR_ReturnCode osvrJointClientOptionsAddAlias(
    OSVR_JointClientOpts opts, const char *path, const char *source);

/** @brief Queues up the manual addition of aliases specified in JSON to the
    path tree.
*/
OSVR_JOINTCLIENTKIT_EXPORT OSVR_ReturnCode osvrJointClientOptionsAddAliases(
    OSVR_JointClientOpts opts, const char *aliases);

/** @brief Queues up the manual addition of a string element to the path tree.
*/
OSVR_JOINTCLIENTKIT_EXPORT OSVR_ReturnCode osvrJointClientOptionsAddString(
    OSVR_JointClientOpts opts, const char *path, const char *s);

/** @brief Queues up a trigger for hardware detection.
*/
OSVR_JOINTCLIENTKIT_EXPORT OSVR_ReturnCode
osvrJointClientOptionsTriggerHardwareDetect(OSVR_JointClientOpts opts);

/** @} */

/** @brief Initialize the library, starting up a "joint" context that also
    contains a server.

    @param applicationIdentifier A null terminated string identifying your
    application. Reverse DNS format strongly suggested.
    @param opts The configuration options object for starting the joint server
    operations. Pass NULL/nullptr for default operation: loading of all
    autoload-enabled plugins, and a hardware detection. If a valid pointer is
    passed, the enqueued operations will be performed in-order (the default
    operations will not be performed). Any exceptions thrown will cause the
   initialization to fail, returning a null context.

    @returns Client context - will be needed for subsequent calls
*/
OSVR_JOINTCLIENTKIT_EXPORT OSVR_ClientContext osvrJointClientInit(
    const char applicationIdentifier[], OSVR_JointClientOpts opts);

/** @} */
OSVR_EXTERN_C_END

#endif
