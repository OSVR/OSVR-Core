/** @file
    @brief Header

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
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
#include <osvr/Transform/Transform.h>
#include <osvr/Util/UniquePtr.h>
#include <osvr/Common/BaseDevicePtr.h>
#include <osvr/Common/SystemComponent_fwd.h>

// Library/third-party includes
#include <vrpn_ConnectionPtr.h>
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <json/value.h>

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
        static int VRPN_CALLBACK
        m_handleRoutingMessage(void *userdata, vrpn_HANDLERPARAM p);
        void m_replaceRoutes(std::string const &routes);
        virtual void m_update();

        void m_handleTrackerRouteEntry(std::string const &dest,
                                       Json::Value src);
        void m_handleStringRouteEntry(std::string const &dest, std::string src);
        void m_addAnalogRouter(const char *src, const char *dest, int channel);
        template <typename Predicate>
        void m_addButtonRouter(const char *src, const char *dest,
                               Predicate pred);

        void m_addTrackerRouter(const char *src, const char *dest,
                                boost::optional<int> sensor,
                                transform::Transform const &xform);
        vrpn_ConnectionPtr m_conn;
        std::string const m_host;
        std::vector<RouterEntryPtr> m_routers;

        common::BaseDevicePtr m_systemDevice;
        common::SystemComponent *m_systemComponent;
    };
} // namespace client
} // namespace osvr

#endif // INCLUDED_VRPNContext_h_GUID_CD10DDF9_457C_4884_077E_D0896E4FBFD1
