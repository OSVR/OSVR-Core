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

#ifndef INCLUDED_MessageType_h_GUID_61B56482_02E5_47B5_8CFA_EAF4286F309F
#define INCLUDED_MessageType_h_GUID_61B56482_02E5_47B5_8CFA_EAF4286F309F

// Internal Includes
#include <ogvr/Connection/MessageTypePtr.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>

namespace ogvr {
/// @brief Base class for connection-specific message type registration
class MessageType : boost::noncopyable {
  public:
    /// @brief destructor
    virtual ~MessageType();

    /// @brief accessor for message name
    std::string const &getName() const;

  protected:
    /// @brief Constructor for use by derived classes only.
    MessageType(std::string const &name);

  private:
    std::string const m_name;
};
} // namespace ogvr
#endif // INCLUDED_MessageType_h_GUID_61B56482_02E5_47B5_8CFA_EAF4286F309F
