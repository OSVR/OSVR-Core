/** @file
    @brief Header shared between multiple C API headers.

    Must be c-safe!

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>;
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

#ifndef INCLUDED_CommonC_h_GUID_F7DD3E37_D90E_4FB4_0BB5_907FDB52F9B7
#define INCLUDED_CommonC_h_GUID_F7DD3E37_D90E_4FB4_0BB5_907FDB52F9B7

/* Internal Includes */
#include <ogvr/PluginKit/Export.h>

/* Library/third-party includes */
#include <libfunctionality/PluginInterface.h>

/* Standard includes */
/* none */

#ifdef __cplusplus
#define OGVR_C_ONLY(X)
#define OGVR_CPP_ONLY(X) X
#else
#define OGVR_C_ONLY(X) X
#define OGVR_CPP_ONLY(X)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @name Return Codes
    @ingroup plugin_c_api
    @{
*/
/** @brief Return type from C plugin API OGVR functions. */
typedef char OGVR_PluginReturnCode;
/** @brief The "success" value for an OGVR_PluginReturnCode */
#define OGVR_PLUGIN_SUCCESS LIBFUNC_RETURN_SUCCESS
/** @brief The "failure" value for an OGVR_PluginReturnCode */
#define OGVR_PLUGIN_FAILURE LIBFUNC_RETURN_FAILURE
/** @} */

/** @brief A context pointer passed in to your plugin's entry point
    @ingroup plugin_c_api
*/
typedef void *OGVR_PluginRegContext;

#ifdef __cplusplus
} /* end of extern "C" */
#endif

#endif
