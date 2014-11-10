/** @file
    @brief Header forward-declaring Connection and specifying the smart pointer
   to hold a Connection in.

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

#ifndef INCLUDED_ConnectionPtr_h_GUID_55F79639_DA70_4BBA_A11C_84797F38288E
#define INCLUDED_ConnectionPtr_h_GUID_55F79639_DA70_4BBA_A11C_84797F38288E

// Internal Includes
#include <ogvr/Util/SharedPtr.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace ogvr {
namespace connection {
    class Connection;
    /// @brief How one must hold a Connection.
    typedef shared_ptr<Connection> ConnectionPtr;
} // namespace connection
} // namespace ogvr

#endif // INCLUDED_ConnectionPtr_h_GUID_55F79639_DA70_4BBA_A11C_84797F38288E
