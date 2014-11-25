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
#include <osvr/PluginKit/Export.h>
#include <osvr/PluginKit/CommonC.h>
#include <osvr/Util/PluginCallbackTypesC.h>
#include <osvr/Util/AnnotationMacrosC.h>

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

OSVR_EXTERN_C_BEGIN

/** @brief This macro begins the entry point function of your plugin.

    Treat it as if it were a function declaration, since that is what it will
   expand to. The function body you write calls some subset of the plugin
   registration methods, then returns either success (OSVR_RETURN_SUCCESS)
   or failure (OSVR_RETURN_FAILURE).

    Your function body receives a single argument, of type
   OSVR_PluginRegContext, named `ctx`. You will need to pass this to most @ref
   PluginKit functions that you call.
*/
#define OSVR_PLUGIN(PLUGIN_NAME) LIBFUNC_PLUGIN(PLUGIN_NAME, ctx)

/** @name Hardware Detection and Driver Instantiation
    @brief If your plugin contains drivers for devices that you can detect,
    you'll want to register for hardware detection. Whether or not you can
   detect, you may wish to register constructors (instantiation callbacks) that
   accept parameters.
    @{
*/

/** @brief Register a callback in your plugin to be notified when hardware
   should be detected again.

   When your callback, a function of type OSVR_HardwareDetectCallback, is
   invoked,
   it will receive the same userdata you provide here (if any). Your plugin
   should do whatever probing necessary to detect devices you can handle and
   instantiate the device drivers.

   @param ctx The registration context passed to your entry point.
   @param detectCallback The address of your callback function
   @param userData An optional opaque pointer that will be returned to you when
   the callback you register here is called.
*/
OSVR_PLUGINKIT_EXPORT OSVR_ReturnCode osvrPluginRegisterHardwareDetectCallback(
    OSVR_INOUT_PTR OSVR_PluginRegContext ctx,
    OSVR_IN OSVR_HardwareDetectCallback detectCallback,
    OSVR_IN_OPT void *userData OSVR_CPP_ONLY(= NULL)) OSVR_FUNC_NONNULL((1));

/** @brief Register an instantiation callback (constructor) for a driver type.
    The given constructor may be called with a string containing configuration
    information, the format of which you should document with your plugin. JSON
    is recommended.

    @param ctx The plugin registration context received by your entry point
    function.
    @param name A unique name for the driver type. The library makes a copy of
    this string.
    @param cb Your callback
    @param userdata An opaque pointer passed to your callback, if desired.
*/
OSVR_PLUGINKIT_EXPORT OSVR_ReturnCode osvrRegisterDriverInstantiationCallback(
    OSVR_INOUT_PTR OSVR_PluginRegContext ctx, OSVR_IN_STRZ const char *name,
    OSVR_IN_PTR OSVR_DriverInstantiationCallback cb,
    OSVR_IN_OPT void *userData OSVR_CPP_ONLY(= NULL))
    OSVR_FUNC_NONNULL((1, 2, 3));

/** @} */

/** @name Plugin Instance Data
    @brief Plugins "own" the modules instantiated in them. Lifetime must be
    managed appropriately: destroyed on shutdown.

    You can store the instances in any way you would like, as long as you
    register them with appropriate deleter callbacks here.

    @{
*/

/** @brief Register plugin data along with an appropriate deleter callback.

    When your callback, a function of type OSVR_PluginDataDeleteCallback, is
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
OSVR_PLUGINKIT_EXPORT OSVR_ReturnCode osvrPluginRegisterDataWithDeleteCallback(
    OSVR_INOUT_PTR OSVR_PluginRegContext ctx,
    OSVR_IN OSVR_PluginDataDeleteCallback deleteCallback,
    OSVR_INOUT_PTR void *pluginData) OSVR_FUNC_NONNULL((1, 2, 3));
/** @} */

OSVR_EXTERN_C_END

/** @} */

#endif
