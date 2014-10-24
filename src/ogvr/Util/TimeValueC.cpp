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
#if defined(OGVR_HAVE_STRUCT_TIMEVAL_IN_SYS_TIME_H)
#include <sys/time.h>
#elif defined(OGVR_HAVE_STRUCT_TIMEVAL_IN_WINSOCK2_H)
#include <winsock2.h>
#endif

#define OGVR_USEC_PER_SEC 1000000;

void ogvrTimeValueNormalize(struct OGVR_TimeValue *tv) {
    if (!tv) {
        return;
    }
    const OGVR_TimeValue_Microseconds rem =
        tv->microseconds / OGVR_USEC_PER_SEC;
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

void ogvrTimeValueSum(struct OGVR_TimeValue *tvA,
                      const struct OGVR_TimeValue *tvB) {
    if (!tvA || !tvB) {
        return;
    }
    tvA->seconds += tvB->seconds;
    tvA->microseconds += tvB->microseconds;
    ogvrTimeValueNormalize(tvA);
}

void ogvrTimeValueDifference(struct OGVR_TimeValue *tvA,
                             const struct OGVR_TimeValue *tvB) {
    if (!tvA || !tvB) {
        return;
    }
    tvA->seconds -= tvB->seconds;
    tvA->microseconds -= tvB->microseconds;
    ogvrTimeValueNormalize(tvA);
}

#ifdef OGVR_HAVE_STRUCT_TIMEVAL
void ogvrTimeValueToStructTimeval(struct timeval *dest,
                                  const struct OGVR_TimeValue *src) {
    if (!dest || !src) {
        return;
    }
    dest->tv_sec = src->seconds;
    dest->tv_usec = src->microseconds;
}

void ogvrStructTimevalToTimeValue(struct OGVR_TimeValue *dest,
                                  const struct timeval *src) {
    if (!dest || !src) {
        return;
    }
    dest->seconds = src->tv_sec;
    dest->microseconds = src->tv_usec;
    ogvrTimeValueNormalize(dest);
}

#endif
