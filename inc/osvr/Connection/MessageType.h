/** @file
    @brief Header

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
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
