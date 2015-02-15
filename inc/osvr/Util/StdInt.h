/** @file
    @brief Header wrapping the C99 standard `stdint` header.

    Must be c-safe!

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

/*
// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)
*/

#ifndef INCLUDED_StdInt_h_GUID_C1AAF35C_C704_4DB7_14AC_615730C4619B
#define INCLUDED_StdInt_h_GUID_C1AAF35C_C704_4DB7_14AC_615730C4619B

#if !defined(_MSC_VER) || (defined(_MSC_VER) && _MSC_VER > 1600)
#include <stdint.h>
#else
#include "MSStdIntC.h"
#endif

#endif
