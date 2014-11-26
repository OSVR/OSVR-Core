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
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>

namespace osvr {
namespace client {
    class RouterEntry : boost::noncopyable {
      public:
        std::string const &getDest() { return m_dest; }
        ClientContext *getContext() { return m_ctx; }
        virtual ~RouterEntry();
        virtual void operator()() = 0;

      protected:
        RouterEntry(ClientContext *ctx, std::string const &dest)
            : m_ctx(ctx), m_dest(dest) {}

      private:
        ClientContext *m_ctx;
        const std::string m_dest;
    };

    typedef unique_ptr<RouterEntry> RouterEntryPtr;

    class VRPNContext : public ::OSVR_ClientContextObject {
      public:
        VRPNContext(const char appId[], const char host[] = "localhost");
        virtual ~VRPNContext();

      private:
        virtual void m_update();
        template <typename Predicate>
        void m_addTrackerRouter(const char *src, const char *dest,
                                Predicate pred);
        vrpn_ConnectionPtr m_conn;
        std::string const m_host;
        std::vector<RouterEntryPtr> m_routers;
    };
} // namespace client
} // namespace osvr

#endif // INCLUDED_VRPNContext_h_GUID_CD10DDF9_457C_4884_077E_D0896E4FBFD1
