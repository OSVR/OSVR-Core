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

#ifndef INCLUDED_AnyMap_h_GUID_794993EB_B778_4E88_16A7_3A047A615587
#define INCLUDED_AnyMap_h_GUID_794993EB_B778_4E88_16A7_3A047A615587

// Internal Includes
// - none

// Library/third-party includes
#include <boost/any.hpp>

// Standard includes
#include <string>
#include <map>

namespace ogvr {
/// @brief A data structure storing "any" by name, to reduce coupling.
typedef std::map<std::string, boost::any> AnyMap;
} // end of namespace ogvr

#endif // INCLUDED_AnyMap_h_GUID_794993EB_B778_4E88_16A7_3A047A615587
