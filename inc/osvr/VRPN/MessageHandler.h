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

#ifndef INCLUDED_MessageHandler_h_GUID_80F936D5_D388_44BF_8BEF_18042312E53A
#define INCLUDED_MessageHandler_h_GUID_80F936D5_D388_44BF_8BEF_18042312E53A

// Internal Includes
#include <osvr/Util/UniquePtr.h>
#include <osvr/Util/TimeValue.h>
#include <osvr/VRPN/Export.h>

// Library/third-party includes
#include <vrpn_ConnectionPtr.h>
#include <boost/noncopyable.hpp>

// Standard includes
#include <functional>
#include <string>
#include <vector>

namespace osvr {
namespace vrpn {

    /// params sender, msgType, timestamp, msg
    typedef std::function<void(std::string const &, std::string const &,
                               OSVR_TimeValue const &, std::string const &)>
        MessageHandlerCallback;
    class CallbackRecord;
    class MessageHandler : boost::noncopyable {
      public:
        OSVR_VRPN_EXPORT MessageHandler(vrpn_ConnectionPtr conn);
        OSVR_VRPN_EXPORT ~MessageHandler();
        OSVR_VRPN_EXPORT void
        registerCallback(MessageHandlerCallback const &cb,
                         std::string const &device = std::string(),
                         std::string const &messageType = std::string());

      private:
        vrpn_ConnectionPtr m_vrpnConnection;
        std::vector<unique_ptr<CallbackRecord> > m_callbackRecords;
    };
} // namespace vrpn
} // namespace osvr

#endif // INCLUDED_MessageHandler_h_GUID_80F936D5_D388_44BF_8BEF_18042312E53A
