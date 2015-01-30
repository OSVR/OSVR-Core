/** @file
    @brief Header

    Must be c-safe!

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

/*
// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)
*/

#ifndef INCLUDED_ChannelCountC_h_GUID_CF7E5EE7_28B0_4B99_E823_DD701904B5D1
#define INCLUDED_ChannelCountC_h_GUID_CF7E5EE7_28B0_4B99_E823_DD701904B5D1

/* Internal Includes */
#include <osvr/Util/StdInt.h>
#include <osvr/Util/APIBaseC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

/** @addtogroup PluginKit
@{
*/

/** @brief The integer type specifying a number of channels/sensors or a
channel/sensor index.
*/
typedef uint32_t OSVR_ChannelCount;

/** @} */

OSVR_EXTERN_C_END

#endif
