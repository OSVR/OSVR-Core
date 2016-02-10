/** @file
    @brief Implementation

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include "GetSerialPortState.h"
#include <osvr/Util/MacroToolsC.h>
#include <osvr/Util/Finally.h>

// Library/third-party includes
// - none

// Standard includes
#include <stdexcept>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

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
    auto f = osvr::util::finally([&hCom] { CloseHandle(hCom); });

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
        return SERIAL_MISSING;
    }
    return getSerialPortStateImpl(port);
}

std::string normalizeSerialPort(std::string const &port) {
    if (port.empty()) {
        return port;
    }
#ifdef _WIN32
    // Use the Win32 device namespace, if they aren't already.
    // Have to double the backslashes because they're escape characters.
    /// @todo device namespace only valid on WinNT-derived windows?
    if (port.find('\\') == std::string::npos) {
        return "\\\\.\\" + port;
    }
#endif
    return port;
}

void verifySerialPort(std::string const &port, std::string const &origPort) {
    std::string const *origPortPtr = origPort.empty() ? &port : &origPort;
    auto portState = getSerialPortState(port);
    if (SERIAL_BUSY == portState) {
        throw std::runtime_error("Cannot access " + *origPortPtr +
                                 ": currently busy. Do you have another "
                                 "application open using that port?");
    } else if (SERIAL_MISSING == portState) {
        throw std::runtime_error("Cannot access " + *origPortPtr +
                                 ": port apparently not found. Make sure the "
                                 "device is plugged in and you've specified "
                                 "the right device and the right port.");
    } else if (SERIAL_INVALID == portState) {
        throw std::runtime_error("Cannot access serial port '" + *origPortPtr +
                                 "': apparently invalid.");
    } else if (SERIAL_AVAILABLE != portState) {
        /// safety catch-all
        throw std::runtime_error("Cannot access serial port '" + *origPortPtr +
                                 "'");
    }
}

std::string normalizeAndVerifySerialPort(std::string const &port) {
    std::string ret = normalizeSerialPort(port);
    verifySerialPort(ret, port);

    return ret;
}
