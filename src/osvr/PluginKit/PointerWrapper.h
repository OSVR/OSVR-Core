/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_PointerWrapper_h_GUID_0C8888FE_163F_46A3_A9F2_1EFCF610C64D
#define INCLUDED_PointerWrapper_h_GUID_0C8888FE_163F_46A3_A9F2_1EFCF610C64D

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
// - none

/// @brief A wrapper to easily define structures that exist just to hold a
/// pointer.
template <typename Contained> class PointerWrapper {
  public:
    PointerWrapper(Contained *ptr = nullptr) : m_ptr(ptr) {}

    Contained **getContainerLocation() { return &m_ptr; }
    Contained *operator->() { return m_ptr; }
    Contained &operator*() { return *m_ptr; }

  private:
    Contained *m_ptr;
};

#endif // INCLUDED_PointerWrapper_h_GUID_0C8888FE_163F_46A3_A9F2_1EFCF610C64D
