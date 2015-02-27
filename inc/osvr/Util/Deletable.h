/** @file
    @brief Header defining a base class for objects that just need to be
   generically deletable.

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

#ifndef INCLUDED_Deletable_h_GUID_E25AB7BA_D9EB_48AD_9BAC_649D03531726
#define INCLUDED_Deletable_h_GUID_E25AB7BA_D9EB_48AD_9BAC_649D03531726

// Internal Includes
#include <osvr/Util/Export.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace util {
    /// @brief Base class for objects that just need to be generically
    /// deletable.
    class Deletable {
      public:
        OSVR_UTIL_EXPORT virtual ~Deletable();
    };
} // namespace util
} // namespace osvr

#endif // INCLUDED_Deletable_h_GUID_E25AB7BA_D9EB_48AD_9BAC_649D03531726
