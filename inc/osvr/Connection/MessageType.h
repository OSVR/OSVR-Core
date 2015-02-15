/** @file
    @brief Header

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
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
#include <osvr/Connection/MessageTypePtr.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>

/// @brief Base class for connection-specific message type registration
struct OSVR_MessageTypeObject : boost::noncopyable {
  public:
    /// @brief destructor
    virtual ~OSVR_MessageTypeObject();

    /// @brief accessor for message name
    std::string const &getName() const;

  protected:
    /// @brief Constructor for use by derived classes only.
    OSVR_MessageTypeObject(std::string const &name);

  private:
    std::string const m_name;
};
#endif // INCLUDED_MessageType_h_GUID_61B56482_02E5_47B5_8CFA_EAF4286F309F
