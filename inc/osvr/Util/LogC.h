/** @file
    @brief Header

    Must be c-safe!

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

/*
// Copyright 2016 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
*/

#ifndef INCLUDED_LogC_h_GUID_A5920837_7CAB_449C_5979_F353138A0122
#define INCLUDED_LogC_h_GUID_A5920837_7CAB_449C_5979_F353138A0122

/* Internal Includes */
#include <osvr/Util/APIBaseC.h>
#include <osvr/Util/LogLevelC.h>
#include <osvr/Util/Export.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

typedef struct OSVR_LoggerObj *OSVR_Logger;
OSVR_UTIL_EXPORT OSVR_Logger osvrLogMakeLogger(const char *loggerName);
OSVR_UTIL_EXPORT void osvrLogFreeLogger(OSVR_Logger logger);
OSVR_UTIL_EXPORT void osvrLogMessage(OSVR_Logger logger, OSVR_LogLevel level,
                                     const char *msg);
OSVR_UTIL_EXPORT OSVR_ReturnCode osvrLogFlush(OSVR_Logger logger);

OSVR_EXTERN_C_END

#endif
