/** @file
    @brief Implementation

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>;
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <ogvr/Util/TimeValueC.h>

// Library/third-party includes
// - none

// Standard includes
// - none

#define OGVR_USEC_PER_SEC 1000000;

void ogvrTimeValueNormalize(struct OGVR_TimeValue *tv) {
    if (!tv) {
        return;
    }
    const int64_t rem = tv->microseconds / OGVR_USEC_PER_SEC;
    tv->seconds += rem;
    tv->microseconds -= rem * OGVR_USEC_PER_SEC;
    /* By here, abs(microseconds) < OGVR_USEC_PER_SEC:
       now let's get signs the same. */
    if (tv->seconds > 0 && tv->microseconds < 0) {
        tv->seconds--;
        tv->microseconds += OGVR_USEC_PER_SEC;
    } else if (tv->seconds < 0 && tv->microseconds > 0) {
        tv->seconds++;
        tv->microseconds -= OGVR_USEC_PER_SEC;
    }
}