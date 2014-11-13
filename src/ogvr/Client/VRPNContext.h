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

#ifndef INCLUDED_VRPNContext_h_GUID_CD10DDF9_457C_4884_077E_D0896E4FBFD1
#define INCLUDED_VRPNContext_h_GUID_CD10DDF9_457C_4884_077E_D0896E4FBFD1

// Internal Includes
#include <ogvr/Client/ClientContext.h>

// Library/third-party includes
// - none

// Standard includes
#include <string>

namespace ogvr {
namespace client {
    class VRPNContext : public ::OGVR_ClientContextObject {
      public:
        VRPNContext(const char appId[], const char host[] = "localhost");
        virtual ~VRPNContext();

      private:
        virtual void m_update();
        std::string const m_host;
    };
} // namespace client
} // namespace ogvr

#endif // INCLUDED_VRPNContext_h_GUID_CD10DDF9_457C_4884_077E_D0896E4FBFD1
