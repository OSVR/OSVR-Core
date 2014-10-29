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

#ifndef INCLUDED_DeviceToken_h_GUID_428B015C_19A2_46B0_CFE6_CC100763D387
#define INCLUDED_DeviceToken_h_GUID_428B015C_19A2_46B0_CFE6_CC100763D387

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <string>

namespace ogvr {
	class AsyncDeviceToken;
	class SyncDeviceToken;
	class DeviceToken {
	public:
		DeviceToken(std::string const& name);
		virtual ~DeviceToken();

		virtual AsyncDeviceToken * asAsyncDevice();
		virtual SyncDeviceToken * asSyncDevice();

	};
} // end of namespace ogvr

#endif // INCLUDED_DeviceToken_h_GUID_428B015C_19A2_46B0_CFE6_CC100763D387
