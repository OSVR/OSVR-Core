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

#ifndef INCLUDED_RawMessageType_h_GUID_839F0B4F_18AC_468C_31DB_33601F95DA3C
#define INCLUDED_RawMessageType_h_GUID_839F0B4F_18AC_468C_31DB_33601F95DA3C

// Internal Includes
#include <osvr/Common/Export.h>
#include <osvr/Util/StdInt.h>

// Library/third-party includes
#include <boost/optional.hpp>

// Standard includes
// - none

namespace osvr {
namespace common {
    class RawMessageType {
      public:
        typedef int32_t UnderlyingMessageType;
        /// @brief Default constructor - "any message type"
        RawMessageType();

        /// @brief Constructor from a registered message type
        explicit RawMessageType(UnderlyingMessageType msg);

        /// @brief Gets the registered message type value or default
        UnderlyingMessageType get() const;

        /// @brief Gets the registered message type value, if specified,
        /// otherwise returns the provided value.
        UnderlyingMessageType getOr(UnderlyingMessageType valueIfNotSet) const;

      private:
        boost::optional<UnderlyingMessageType> m_message;
    };

} // namespace common
} // namespace osvr

#endif // INCLUDED_RawMessageType_h_GUID_839F0B4F_18AC_468C_31DB_33601F95DA3C
