/** @file
    @brief Implementation

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

// Internal Includes
#include "GetSerialPortState.h"
#include <osvr/Util/MacroToolsC.h>

// Library/third-party includes
#include <boost/scope_exit.hpp>

// Standard includes
// - none

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>

// Inspired by answer by Yangghi Min on
// https://social.msdn.microsoft.com/Forums/vstudio/en-US/588b7967-cce4-4412-87e8-593dfeb228b4/getting-the-list-of-available-serial-ports-in-c
inline SerialPortState getSerialPortStateImpl(std::string const &port) {
    GetLastError(); // clear last error
    HANDLE hCom = CreateFile(port.c_str(), GENERIC_READ | GENERIC_WRITE,
                             0,             // non-sharing mode
                             nullptr,       // ignore security
                             OPEN_EXISTING, // required for com ports
                             0, // just basic open for existence test.
                             nullptr);

    /// auto-close handle.
    BOOST_SCOPE_EXIT(&hCom) { CloseHandle(hCom); }
    BOOST_SCOPE_EXIT_END

    if (INVALID_HANDLE_VALUE == hCom) {
        auto err = GetLastError();
        if (ERROR_ACCESS_DENIED == err) {
            return SERIAL_BUSY;
        }
        return SERIAL_MISSING;
    }
    return SERIAL_AVAILABLE;
}
#else

inline SerialPortState getSerialPortStateImpl(std::string const &port) {
    /// @todo just assuming it's available on platforms where we don't know how
    /// to check.
    return SERIAL_AVAILABLE;
}
#endif

SerialPortState getSerialPortState(std::string const &port) {
    if (port.empty()) {
        return SERIAL_INVALID;
    }
    return getSerialPortStateImpl(port);
}

void requireSerialPortAvailable(std::string const &port, std::string origPort) {
    if (origPort.empty()) {
        origPort = port;
    }
    auto portState = getSerialPortState(port);
    if (SERIAL_BUSY == portState) {
        throw std::runtime_error("Cannot access " + origPort +
                                 ": currently busy. Do you have another "
                                 "application open using that port?");
    } else if (SERIAL_MISSING == portState) {
        throw std::runtime_error("Cannot access " + origPort +
                                 ": port apparently not found. Make sure the "
                                 "device is plugged in and you've specified "
                                 "the right device and the right port.");
    } else if (SERIAL_INVALID == portState) {
        throw std::runtime_error("Cannot access serial port '" + origPort +
                                 "': apparently invalid.");
    } else if (SERIAL_AVAILABLE != portState) {
        /// safety catch-all
        throw std::runtime_error("Cannot access serial port '" + origPort +
                                 "'");
    }
}