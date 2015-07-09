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
    class OSVR_UTIL_EXPORT Deletable {
      public:
        virtual ~Deletable();
    };
} // namespace util
} // namespace osvr

#endif // INCLUDED_Deletable_h_GUID_E25AB7BA_D9EB_48AD_9BAC_649D03531726
