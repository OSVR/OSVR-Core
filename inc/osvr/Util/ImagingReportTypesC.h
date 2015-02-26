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

#ifndef INCLUDED_ImagingReportTypesC_h_GUID_746A7BF8_B92D_4585_CA72_DC5391DEDF24
#define INCLUDED_ImagingReportTypesC_h_GUID_746A7BF8_B92D_4585_CA72_DC5391DEDF24

/* Internal Includes */
#include <osvr/Util/APIBaseC.h>
#include <osvr/Util/StdInt.h>
#include <osvr/Util/BoolC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

/** @addtogroup ClientKit
    @{
*/
typedef uint32_t OSVR_ImageDimension;
typedef uint8_t OSVR_ImageChannels;
typedef uint8_t OSVR_ImageDepth;

/** @brief Type for raw buffer access to image data */
typedef unsigned char OSVR_ImageBufferElement;

typedef enum OSVR_ImagingValueType {
    OSVR_IVT_UNSIGNED_INT = 0,
    OSVR_IVT_SIGNED_INT = 1,
    OSVR_IVT_FLOATING_POINT = 2
} OSVR_ImagingValueType;

typedef struct OSVR_ImagingMetadata {
    /** @brief height in pixels */
    OSVR_ImageDimension height;
    /** @brief width in pixels */
    OSVR_ImageDimension width;
    /** @brief number of channels of data for each pixel */
    OSVR_ImageChannels channels;
    /** @brief the depth (size) in bytes of each channel - valid values are 1,
     * 2, 4, and 8 */
    OSVR_ImageDepth depth;
    /** @brief Whether values are unsigned ints, signed ints, or floating point
     */
    OSVR_ImagingValueType type;

} OSVR_ImagingMetadata;

/** @} */

OSVR_EXTERN_C_END

#endif
