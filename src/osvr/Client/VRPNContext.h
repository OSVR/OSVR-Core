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
#include <osvr/Client/ClientContext.h>
#include <osvr/Util/UniquePtr.h>

// Library/third-party includes
#include <vrpn_ConnectionPtr.h>

// Standard includes
#include <string>

namespace osvr {
namespace client {
    class CallableObject {
      public:
        virtual ~CallableObject();
        virtual void operator()() = 0;
    };
    typedef unique_ptr<CallableObject> CallablePtr;
    class VRPNContext : public ::OSVR_ClientContextObject {
      public:
        VRPNContext(const char appId[], const char host[] = "localhost");
        virtual ~VRPNContext();

      private:
        virtual void m_update();
        vrpn_ConnectionPtr m_conn;
        std::string const m_host;
        std::vector<CallablePtr> m_routers;
    };
} // namespace client
} // namespace osvr

#endif // INCLUDED_VRPNContext_h_GUID_CD10DDF9_457C_4884_077E_D0896E4FBFD1
