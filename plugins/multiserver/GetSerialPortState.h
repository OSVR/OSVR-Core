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

#ifndef INCLUDED_GetSerialPortState_h_GUID_6A7FD323_CCB5_4093_45E3_9BA7A2943777
#define INCLUDED_GetSerialPortState_h_GUID_6A7FD323_CCB5_4093_45E3_9BA7A2943777

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <string>

enum SerialPortState {
    SERIAL_INVALID,  ///< Something's wrong with the port name you gave
    SERIAL_MISSING,  ///< serial port requested is missing.
    SERIAL_BUSY,     ///< serial port requested is present, but busy (opened for
                     ///exclusive use).
    SERIAL_AVAILABLE ///< serial port requested is available and not opened for
                     ///exclusive use.
};

/// @brief Returns an enum value indicating the apparent state of the port.
SerialPortState getSerialPortState(std::string const &port);

/// @brief Checks the state of the serial port and throws a std::exception
/// unless it is available.
///
/// @param port The serial port name
/// @param origPort Optionally, the port as it should be included in the
/// exception text (for instance, before prefixing \\.\ on Windows). If empty
/// (default), will use @p port.
void requireSerialPortAvailable(std::string const &port,
                                std::string origPort = std::string());

#endif // INCLUDED_GetSerialPortState_h_GUID_6A7FD323_CCB5_4093_45E3_9BA7A2943777
