/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>

*/

// Copyright 2015 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
