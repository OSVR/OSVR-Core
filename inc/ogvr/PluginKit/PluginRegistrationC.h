/** @file
    @brief Header

    Must be c-safe!

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>

*/

/*
// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)
*/

#ifndef INCLUDED_PluginRegistrationC_h_GUID_C019DFA9_5B54_4791_B0A4_040EA20501BA
#define INCLUDED_PluginRegistrationC_h_GUID_C019DFA9_5B54_4791_B0A4_040EA20501BA

/* Internal Includes */
#include <ogvr/PluginKit/Export.h>
#include <ogvr/PluginKit/CommonC.h>
#include <ogvr/Util/PluginCallbackTypesC.h>
#include <ogvr/Util/AnnotationMacrosC.h>

/* Library/third-party includes */
#include <libfunctionality/PluginInterface.h>

/* Standard includes */
#include <stddef.h>

/** @defgroup PluginKitCRegistration Plugin Registration (base C API)
    @brief How to start writing a plugin and advertise your capabilities to the
   core library.
    @ingroup PluginKit

   @{
*/

OGVR_EXTERN_C_BEGIN

/** @brief This macro begins the entry point function of your plugin.

    Treat it as if it were a function declaration, since that is what it will
   expand to. The function body you write calls some subset of the plugin
   registration methods, then returns either success (OGVR_RETURN_SUCCESS)
   or failure (OGVR_RETURN_FAILURE).

    Your function body receives a single argument, of type
   OGVR_PluginRegContext, named `ctx`. You will need to pass this to most @ref
   PluginKit functions that you call.
*/
#define OGVR_PLUGIN(PLUGIN_NAME) LIBFUNC_PLUGIN(PLUGIN_NAME, ctx)

/** @name Hardware Polling
    @brief If your plugin contains drivers for devices that you can detect,
    you'll want to register for hardware polling.
    @{
*/

/** @brief Register a callback in your plugin to be notified when hardware
   should be polled again.

   When your callback, a function of type OGVR_HardwarePollCallback, is invoked,
   it will receive the same userdata you provide here (if any). Your plugin
   should do whatever probing necessary to detect devices you can handle and
   instantiate the device drivers.

   @param ctx The registration context passed to your entry point.
   @param pollCallback The address of your callback function
   @param userData An optional opaque pointer that will be returned to you when
   the callback you register here is called.
*/
OGVR_PLUGINKIT_EXPORT OGVR_ReturnCode ogvrPluginRegisterHardwarePollCallback(
    OGVR_INOUT_PTR OGVR_PluginRegContext ctx,
    OGVR_IN OGVRHardwarePollCallback pollCallback,
    OGVR_IN_OPT void *userData OGVR_CPP_ONLY(= NULL)) OGVR_FUNC_NONNULL((1));
/** @} */

/** @name Plugin Instance Data
    @brief Plugins "own" the modules instantiated in them. Lifetime must be
    managed appropriately: destroyed on shutdown.

    You can store the instances in any way you would like, as long as you
    register them with appropriate deleter callbacks here.

    @{
*/

/** @brief Register plugin data along with an appropriate deleter callback.

    When your callback, a function of type OGVR_PluginDataDeleteCallback, is
   invoked, it will receive the plugin data pointer you provide here. Your
   deleter is responsible for appropriately deleting/freeing/destructing all
   data associated with that pointer.

    This function may be called more than once, to register multiple plugin data
   objects. Callbacks will be called, sorted first by plugin, in reverse order
   of registration.

    @param ctx The registration context passed to your entry point.
    @param deleteCallback The address of your deleter callback function
    @param pluginData A pointer to your data, treated as opaque by this library,
   and passed to your deleter.
*/
OGVR_PLUGINKIT_EXPORT OGVR_ReturnCode ogvrPluginRegisterDataWithDeleteCallback(
    OGVR_INOUT_PTR OGVR_PluginRegContext ctx,
    OGVR_IN OGVR_PluginDataDeleteCallback deleteCallback,
    OGVR_INOUT_PTR void *pluginData) OGVR_FUNC_NONNULL((1, 2, 3));
/** @} */

OGVR_EXTERN_C_END

/** @} */

#endif
