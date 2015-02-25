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

#ifndef INCLUDED_RawSenderType_h_GUID_DF724F49_FE2F_4EB4_8CFC_8C16424400EC
#define INCLUDED_RawSenderType_h_GUID_DF724F49_FE2F_4EB4_8CFC_8C16424400EC

// Internal Includes
#include <osvr/Common/Export.h>
#include <osvr/Util/StdInt.h>

// Library/third-party includes
#include <boost/optional.hpp>

// Standard includes
// - none

namespace osvr {
namespace common {
    class RawSenderType {
      public:
        typedef int32_t UnderlyingSenderType;

        /// @brief Default constructor - "any sender"
        RawSenderType();

        /// @brief Constructor from a registered sender
        explicit RawSenderType(UnderlyingSenderType sender);

        /// @brief Gets the registered sender value or default
        UnderlyingSenderType get() const;

        /// @brief Gets the registered sender value, if specified, otherwise
        /// returns the provided value.
        UnderlyingSenderType getOr(UnderlyingSenderType valueIfNotSet) const;

      private:
        boost::optional<UnderlyingSenderType> m_sender;
    };

} // namespace common
} // namespace osvr
#endif // INCLUDED_RawSenderType_h_GUID_DF724F49_FE2F_4EB4_8CFC_8C16424400EC
