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

#ifndef INCLUDED_ClientMainloop_h_GUID_CE5D82DA_5A69_46A0_55BA_BE9DB68B61CE
#define INCLUDED_ClientMainloop_h_GUID_CE5D82DA_5A69_46A0_55BA_BE9DB68B61CE

// Internal Includes
#include <osvr/ClientKit/ClientKit.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>

// Standard includes
// - none

/// @brief Simple class to handle running a client mainloop in another thread,
/// but easily pausable.
class ClientMainloop : boost::noncopyable {
  public:
    ClientMainloop(osvr::clientkit::ClientContext &ctx) : m_ctx(ctx) {}
    void mainloop() {
        boost::unique_lock<boost::mutex> lock(m_mutex, boost::try_to_lock);
        if (lock) {
            m_ctx.update();
        }
    }
    boost::mutex &getMutex() { return m_mutex; }

  private:
    osvr::clientkit::ClientContext &m_ctx;
    boost::mutex m_mutex;
};

#endif // INCLUDED_ClientMainloop_h_GUID_CE5D82DA_5A69_46A0_55BA_BE9DB68B61CE
