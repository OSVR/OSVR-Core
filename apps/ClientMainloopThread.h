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
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_ClientMainloopThread_h_GUID_552DB227_9D15_4B9D_E3CD_42A57C734029
#define INCLUDED_ClientMainloopThread_h_GUID_552DB227_9D15_4B9D_E3CD_42A57C734029

// Internal Includes
#include "ClientMainloop.h"

// Library/third-party includes
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/chrono.hpp>

// Standard includes
#include <stdexcept>

static const auto SLEEP_TIME = boost::posix_time::milliseconds(1);

class ClientMainloopThread : boost::noncopyable {
  public:
    ClientMainloopThread(osvr::clientkit::ClientContext &ctx,
                         bool startNow = false)
        : m_run(false), m_started(false), m_mainloop(ctx) {
        if (startNow) {
            start();
        }
    }
    void start() {
        if (m_run || m_started) {
            throw std::logic_error(
                "Can't start if it's already started or if this is a re-start");
        }
        m_started = true;
        m_run = true;
        m_thread = boost::thread([&] {
            while (m_run) {
                oneLoop();
            }
        });
    }

    void oneLoop() {
        m_mainloop.mainloop();
        boost::this_thread::sleep(SLEEP_TIME);
    }

    template <typename T>
    void loopForDuration(T duration = boost::chrono::seconds(2)) {
        typedef boost::chrono::steady_clock clock;
        auto start = clock::now();
        do {
            oneLoop();
        } while (clock::now() - start < duration);
    }

    ~ClientMainloopThread() {
        m_run = false;
        m_thread.join();
    }
    boost::mutex &getMutex() { return m_mainloop.getMutex(); }

  private:
    volatile bool m_run;
    bool m_started;
    ClientMainloop m_mainloop;
    boost::thread m_thread;
};

#endif // INCLUDED_ClientMainloopThread_h_GUID_552DB227_9D15_4B9D_E3CD_42A57C734029
