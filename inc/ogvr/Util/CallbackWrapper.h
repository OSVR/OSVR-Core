/** @file
    @brief Header

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>;
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_CallbackWrapper_h_GUID_6169ADE2_5BA1_4A81_47C9_9E492F6405ED
#define INCLUDED_CallbackWrapper_h_GUID_6169ADE2_5BA1_4A81_47C9_9E492F6405ED

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
// - none

namespace ogvr {
template <typename FunctionType> class CallbackWrapper {

  private:
    FunctionType m_f;
};
} // end of namespace ogvr
#endif // INCLUDED_CallbackWrapper_h_GUID_6169ADE2_5BA1_4A81_47C9_9E492F6405ED
