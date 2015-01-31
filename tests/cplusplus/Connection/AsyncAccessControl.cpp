/** @file
    @brief Test Implementation

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

// Internal Includes
#include "../../../src/osvr/Connection/AsyncAccessControl.h"
#include "../../../src/osvr/Connection/AsyncAccessControl.cpp"

// Library/third-party includes
#include "gtest/gtest.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/scoped_thread.hpp>

// Standard includes
// - none

using std::string;
using namespace osvr::connection;
const auto WAIT_TIME = boost::posix_time::seconds(1);

inline void pleaseYield() { boost::this_thread::yield(); }
inline void pleaseSleep() { boost::this_thread::sleep(WAIT_TIME); }

TEST(AsyncAccessControl, simple) {
    AsyncAccessControl control;
    volatile bool sent = false;
    ASSERT_FALSE(control.mainThreadCTS())
        << "CTS should have no tasks waiting.";

    boost::scoped_thread<> asyncThread([&] {
        RequestToSend rts(control);
        ASSERT_TRUE(rts.request()) << "Request should be approved";
        ASSERT_FALSE(rts.isNested());
        sent = true;
    });

    pleaseSleep();
    ASSERT_FALSE(sent) << "Shouldn't have been permitted to send yet.";
    while (!control.mainThreadCTS()) {
        pleaseYield();
    }
    ASSERT_TRUE(sent) << "Should have sent";

    ASSERT_FALSE(control.mainThreadCTS())
        << "CTS should have no tasks waiting.";
}

TEST(AsyncAccessControl, serialRequests) {
    AsyncAccessControl control;
    volatile bool sent1 = false;
    volatile bool sent2 = false;
    ASSERT_FALSE(control.mainThreadCTS())
        << "CTS should have no tasks waiting.";

    boost::scoped_thread<> asyncThread([&] {
        {
            RequestToSend rts(control);
            ASSERT_TRUE(rts.request()) << "Request should be approved";
            ASSERT_FALSE(rts.isNested());
            sent1 = true;
        }
        pleaseSleep();
        {
            RequestToSend rts(control);
            ASSERT_TRUE(rts.request()) << "Request should be approved";
            ASSERT_FALSE(rts.isNested());
            sent2 = true;
        }

    });

    pleaseSleep();
    ASSERT_FALSE(sent1) << "Shouldn't have been permitted to send first yet.";
    ASSERT_FALSE(sent2) << "Shouldn't have been permitted to send second yet.";

    while (!control.mainThreadCTS()) {
        pleaseYield();
    }
    ASSERT_TRUE(sent1) << "Should have sent first.";
    ASSERT_FALSE(sent2) << "Shouldn't have been permitted to send second yet.";

    while (!control.mainThreadCTS()) {
        pleaseYield();
    }
    ASSERT_TRUE(sent1) << "Should have sent first.";
    ASSERT_TRUE(sent2) << "Should have sent second.";

    ASSERT_FALSE(control.mainThreadCTS())
        << "CTS should have no tasks waiting.";
}

TEST(AsyncAccessControl, recursive) {
    AsyncAccessControl control;
    volatile bool outer = false;
    volatile bool inner = false;
    ASSERT_FALSE(control.mainThreadCTS())
        << "CTS should have no tasks waiting.";

    boost::scoped_thread<> asyncThread([&] {
        RequestToSend rts(control);
        ASSERT_TRUE(rts.request()) << "Request should be approved";
        ASSERT_FALSE(rts.isNested());
        outer = true;
        {
            RequestToSend rts2(control);
            ASSERT_TRUE(rts2.request())
                << "Request should be approved since we're already in it.";
            inner = true;
            ASSERT_TRUE(rts2.isNested());
        }
    });

    pleaseSleep();
    ASSERT_FALSE(outer || inner)
        << "Shouldn't have been permitted to send yet.";
    while (!control.mainThreadCTS()) {
        pleaseYield();
    }
    ASSERT_TRUE(outer) << "Should have gotten outer permission";
    ASSERT_TRUE(inner) << "Should have gotten inner permission";

    ASSERT_FALSE(control.mainThreadCTS())
        << "CTS should have no tasks waiting.";
}