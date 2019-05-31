/** @file
    @brief Test Implementation

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>

*/

// Copyright 2014 Sensics, Inc.
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

// Internal Includes
#include "../../../src/osvr/Connection/AsyncAccessControl.h"
#include "../../../src/osvr/Connection/AsyncAccessControl.cpp"

// Library/third-party includes
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <catch2/catch.hpp>

// Standard includes
#include <memory>

using std::string;
using namespace osvr::connection;
const auto WAIT_TIME = boost::posix_time::seconds(1);

inline void pleaseYield() { boost::this_thread::yield(); }
inline void pleaseSleep() { boost::this_thread::sleep(WAIT_TIME); }

/// @brief Quick hack to avoid having to depend on boost::scoped_thread and thus
/// boost > 1.49.0
class ScopedThread : boost::noncopyable {
  public:
    typedef std::unique_ptr<boost::thread> ThreadHolder;
    ScopedThread(boost::thread *t) : m_thread(t) {}
    ~ScopedThread() {
        if (m_thread) {
            m_thread->join();
        }
    }

  private:
    ThreadHolder m_thread;
};

TEST_CASE("AsyncAccessControl-simple") {
    AsyncAccessControl control;
    volatile bool sent = false;
    {
        INFO("CTS should have no tasks waiting.");
        REQUIRE_FALSE(control.mainThreadCTS());
    }

    ScopedThread asyncThread(new boost::thread([&] {
        RequestToSend rts(control);
        {
            INFO("Request should be approved");
            REQUIRE(rts.request());
        }
        REQUIRE_FALSE(rts.isNested());
        sent = true;
    }));

    pleaseSleep();
    {
        INFO("Shouldn't have been permitted to send yet.");
        REQUIRE_FALSE(sent);
    }

    while (!control.mainThreadCTS()) {
        pleaseYield();
    }

    {
        INFO("Should have sent");
        REQUIRE(sent);
    }
    {
        INFO("CTS should have no tasks waiting.");
        REQUIRE_FALSE(control.mainThreadCTS());
    }
}

TEST_CASE("AsyncAccessControl-serialRequests") {
    AsyncAccessControl control;
    volatile bool sent1 = false;
    volatile bool sent2 = false;
    {
        INFO("CTS should have no tasks waiting.");
        REQUIRE_FALSE(control.mainThreadCTS());
    }

    ScopedThread asyncThread(new boost::thread([&] {
        {
            RequestToSend rts(control);
            {
                INFO("Request should be approved");
                REQUIRE(rts.request());
            }
            REQUIRE_FALSE(rts.isNested());
            sent1 = true;
        }
        pleaseSleep();
        {
            RequestToSend rts(control);
            {
                INFO("Request should be approved");
                REQUIRE(rts.request());
            }
            REQUIRE_FALSE(rts.isNested());
            sent2 = true;
        }
    }));

    pleaseSleep();
    {
        INFO("Shouldn't have been permitted to send first yet.");
        REQUIRE_FALSE(sent1);
    }
    {
        INFO("Shouldn't have been permitted to send second yet.");
        REQUIRE_FALSE(sent2);
    }

    while (!control.mainThreadCTS()) {
        pleaseYield();
    }
    {
        INFO("Should have sent first.");
        REQUIRE(sent1);
    }
    {
        INFO("Shouldn't have been permitted to send second yet.");
        REQUIRE_FALSE(sent2);
    }

    while (!control.mainThreadCTS()) {
        pleaseYield();
    }
    {
        INFO("Should have sent first.");
        REQUIRE(sent1);
    }
    {
        INFO("Should have sent second.");
        REQUIRE(sent2);
    }
    {
        INFO("CTS should have no tasks waiting.");
        REQUIRE_FALSE(control.mainThreadCTS());
    }
}

TEST_CASE("AsyncAccessControl-recursive") {
    AsyncAccessControl control;
    volatile bool outer = false;
    volatile bool inner = false;
    {
        INFO("CTS should have no tasks waiting.");
        REQUIRE_FALSE(control.mainThreadCTS());
    }

    ScopedThread asyncThread(new boost::thread([&] {
        RequestToSend rts(control);
        {
            INFO("Request should be approved");
            REQUIRE(rts.request());
        }
        REQUIRE_FALSE(rts.isNested());
        outer = true;
        {
            RequestToSend rts2(control);
            {
                INFO("Request should be approved since we're already in it");
                REQUIRE(rts2.request());
            }
            inner = true;
            REQUIRE(rts2.isNested());
        }
    }));

    pleaseSleep();
    {
        INFO("Shouldn't have been permitted to send yet.");
        REQUIRE_FALSE(outer);
        REQUIRE_FALSE(inner);
    }
    while (!control.mainThreadCTS()) {
        pleaseYield();
    }
    {
        INFO("Should have gotten outer permission");
        REQUIRE(outer);
    }
    {
        INFO("Should have gotten inner permission");
        REQUIRE(inner);
    }

    {
        INFO("CTS should have no tasks waiting.");
        REQUIRE_FALSE(control.mainThreadCTS());
    }
}
