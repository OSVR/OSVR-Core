/** @file
    @brief Header declaring the opaque plugin registration context type

    Must be c-safe!

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

/*
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
*/

#ifndef INCLUDED_PluginRegContextC_h_GUID_D52319F2_F80B_46B8_EB90_0575A24B0134
#define INCLUDED_PluginRegContextC_h_GUID_D52319F2_F80B_46B8_EB90_0575A24B0134

/* Internal Includes */
#include <osvr/Util/APIBaseC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

/** @brief A context pointer passed in to your plugin's entry point and other
   locations of control flow transfer into our plugin.
    @ingroup PluginKit
*/
typedef void *OSVR_PluginRegContext;

OSVR_EXTERN_C_END

#endif
