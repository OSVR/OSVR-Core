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

#ifndef INCLUDED_JSONTools_h_GUID_9AB026D8_AE73_47BC_1959_278E34C9B151
#define INCLUDED_JSONTools_h_GUID_9AB026D8_AE73_47BC_1959_278E34C9B151

// Internal Includes
#include <osvr/Util/EigenCoreGeometry.h>

// Library/third-party includes
#include <json/value.h>

// Standard includes
// - none

inline Json::Value toJson(Eigen::Vector3d const &vec) {
    Json::Value ret(Json::arrayValue);
    ret.append(vec[0]);
    ret.append(vec[1]);
    ret.append(vec[2]);
    return ret;
}

#endif // INCLUDED_JSONTools_h_GUID_9AB026D8_AE73_47BC_1959_278E34C9B151
