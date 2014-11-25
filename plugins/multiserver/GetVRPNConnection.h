/** @file
    @brief Header

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_GetVRPNConnection_h_GUID_008AFE04_5378_43B9_BE78_571F7C16D2DD
#define INCLUDED_GetVRPNConnection_h_GUID_008AFE04_5378_43B9_BE78_571F7C16D2DD

// Internal Includes
// - none

// Library/third-party includes
#include <osvr/PluginHost/PluginSpecificRegistrationContext.h>
#include <osvr/Util/PluginRegContextC.h>

// Standard includes
// - none

class vrpn_Connection;

vrpn_Connection *getVRPNConnection(OSVR_PluginRegContext ctx);
vrpn_Connection *
getVRPNConnection(osvr::pluginhost::PluginSpecificRegistrationContext &context);

#endif // INCLUDED_GetVRPNConnection_h_GUID_008AFE04_5378_43B9_BE78_571F7C16D2DD
