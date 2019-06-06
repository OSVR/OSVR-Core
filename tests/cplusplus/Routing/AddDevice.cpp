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
#include "IsType.h"
#include <osvr/Common/AddDevice.h>
#include <osvr/Common/PathTree.h>
#include <osvr/Common/RoutingExceptions.h>

// Library/third-party includes
#include <boost/variant/get.hpp>
#include <catch2/catch.hpp>

// Standard includes
// - none

using std::string;
using namespace osvr::common;

TEST_CASE("addDevice-normalConditions") {
    PathTree tree;
    PathNode *dev = nullptr;
    REQUIRE_NOTHROW(dev = &addDevice(tree, "/com_osvr_sample/MyDevice"));

    // Check Device
    SECTION("Check device") {
        REQUIRE(dev->getName() == "MyDevice");
        REQUIRE_FALSE(dev->hasChildren());
        REQUIRE(isNodeType<elements::DeviceElement>(*dev));
        REQUIRE_FALSE(dev->getParent() == nullptr);
    }

    // Check com_osvr_sample
    auto plugin = dev->getParent();
    SECTION("Check com_osvr_sample") {
        REQUIRE(plugin->getName() == "com_osvr_sample");
        REQUIRE(isNodeType<elements::PluginElement>(*plugin));
        REQUIRE_FALSE(plugin->getParent() == nullptr);
    }
    auto root = plugin->getParent();

    REQUIRE(root->isRoot());
    {
        INFO("Root identity should be preserved");
        REQUIRE(tree.getNodeByPath("/") == *root);
    }
    {
        INFO("Identity should be preserved");
        REQUIRE(tree.getNodeByPath("/com_osvr_sample/MyDevice") == *dev);
    }
}

TEST_CASE("addDevice-missingLeadingSlash") {
    PathTree tree;

    PathNode *dev = nullptr;
    INFO("Should forgive a missing leading slash");
    REQUIRE_NOTHROW(dev = &addDevice(tree, "com_osvr_sample/MyDevice"));

    INFO("Should be the same as if the slash had been present");
    REQUIRE(tree.getNodeByPath("/com_osvr_sample/MyDevice") == *dev);
}

TEST_CASE("addDevice-BadInput") {
    PathTree tree;
    SECTION("Should reject an empty path") {
        REQUIRE_THROWS_AS(addDevice(tree, ""), exceptions::InvalidDeviceName);
    }

    SECTION("Should reject the root") {
        REQUIRE_THROWS_AS(addDevice(tree, "/"), exceptions::InvalidDeviceName);
    }

    SECTION("Should reject just a single level") {
        REQUIRE_THROWS_AS(addDevice(tree, "/com_osvr_sample"),
                          exceptions::InvalidDeviceName);
    }

    SECTION("Should reject just a single level with trailing slash") {
        REQUIRE_THROWS_AS(addDevice(tree, "/com_osvr_sample/"),
                          exceptions::InvalidDeviceName);
    }
    SECTION("Should reject just a single level "
            "w/o leading slash") {
        REQUIRE_THROWS_AS(addDevice(tree, "com_osvr_sample"),
                          exceptions::InvalidDeviceName);
    }

    SECTION("Should reject just a single level with trailing but w/o leading "
            "slash") {
        REQUIRE_THROWS_AS(addDevice(tree, "com_osvr_sample/"),
                          exceptions::InvalidDeviceName);
    }

    SECTION("Should reject empty second level") {
        REQUIRE_THROWS_AS(addDevice(tree, "/com_osvr_sample//"),
                          exceptions::EmptyPathComponent);

        REQUIRE_THROWS_AS(addDevice(tree, "com_osvr_sample//"),
                          exceptions::EmptyPathComponent);
    }

    SECTION("Too short to be legit.")
    REQUIRE_THROWS_AS(addDevice(tree, "//"), exceptions::InvalidDeviceName);

    SECTION("Should reject empty level") {
        REQUIRE_THROWS_AS(addDevice(tree, "///"),
                          exceptions::EmptyPathComponent);
    }
}
