/** @file
    @brief Header

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_GenericCaller_h_GUID_C49A7DEE_89E6_478D_627A_CFFB9ED39EAC
#define INCLUDED_GenericCaller_h_GUID_C49A7DEE_89E6_478D_627A_CFFB9ED39EAC

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
// - none
namespace ogvr {
namespace detail {
    /// @brief Generic nullary function object caller
    ///
    /// Used in header-only C++ wrappers over C APIs.
    template <typename ReturnType, typename FunctionObjectType>
    inline ReturnType generic_caller0(void *userData) {
        FunctionObjectType *o = static_cast<FunctionObjectType *>(userData);
        return (*o)();
    }
} // end of namespace detail
} // end of namespace ogvr

#endif // INCLUDED_GenericCaller_h_GUID_C49A7DEE_89E6_478D_627A_CFFB9ED39EAC
