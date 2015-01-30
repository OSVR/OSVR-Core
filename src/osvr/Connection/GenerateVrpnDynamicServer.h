/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_GenerateVrpnDynamicServer_h_GUID_6828B8BC_E58A_4DAE_E511_46B854C51C11
#define INCLUDED_GenerateVrpnDynamicServer_h_GUID_6828B8BC_E58A_4DAE_E511_46B854C51C11

// Internal Includes
#include <osvr/Connection/DeviceInitObject.h>

// Library/third-party includes
#include <vrpn_MainloopObject.h>

// Standard includes
// - none

namespace osvr {
namespace connection {
    vrpn_MainloopObject *generateVrpnDynamicServer(DeviceInitObject &init,
                                                   vrpn_Connection *conn);
} // namespace connection
} // namespace osvr

#endif // INCLUDED_GenerateVrpnDynamicServer_h_GUID_6828B8BC_E58A_4DAE_E511_46B854C51C11
