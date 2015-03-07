/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com>

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

#ifndef INCLUDED_Microsleep_h_GUID_18728CEA_A634_4268_845E_86007CA19511
#define INCLUDED_Microsleep_h_GUID_18728CEA_A634_4268_845E_86007CA19511

// Internal Includes
#include <osvr/Util/ReturnCodesC.h>
#include <osvr/Util/Export.h>

// Library/third-party includes
#include <boost/thread/thread.hpp>

// Standard includes
// - none

namespace osvr {

namespace util {

    namespace time {

        /**
         * @brief Request a thread sleep for at least the given number of
         * microseconds.
         *
         * DO NOT use within a Sync plugin!
         *
         * This is just a request for a minimum sleep time -- operating system
         * scheduling and sleep granularity means that you may end up sleeping
         * for longer.
         *
         */
        inline OSVR_ReturnCode microsleep(OSVR_IN uint64_t microseconds) {
            boost::this_thread::sleep(
                boost::posix_time::microseconds(microseconds));
            return OSVR_RETURN_SUCCESS;
        }

    } // end namespace time

} // end namespace util

} // end namespace osvr

#endif // INCLUDED_Microsleep_h_GUID_18728CEA_A634_4268_845E_86007CA19511
