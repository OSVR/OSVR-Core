/** @file
    @brief Test Implementation

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

// Internal Includes
#include "DummyTree.h"
#include <osvr/Common/ResolveTreeNode.h>

// Library/third-party includes
#include <catch2/catch.hpp>
#include <json/value.h>

// Standard includes
// - none

namespace common = osvr::common;
using osvr::common::PathTree;

class PathTreeResolution {
  public:
    PathTreeResolution() { dummy::setupDummyDevice(tree); }

    void setAlias(std::string const &alias) {
        tree.getNodeByPath(dummy::getAlias(),
                           common::elements::AliasElement(alias));
    }
    void checkResolution() {
        REQUIRE_NOTHROW(common::resolveTreeNode(tree, dummy::getAlias()));
        REQUIRE(
            true ==
            common::resolveTreeNode(tree, dummy::getAlias()).is_initialized());
        source = *common::resolveTreeNode(tree, dummy::getAlias());

        REQUIRE(source.getInterfaceName() == dummy::getInterface());
        REQUIRE(source.getDeviceElement().getDeviceName() ==
                dummy::getDevice());
        REQUIRE(source.getDeviceElement().getServer() == dummy::getHost());
        REQUIRE( source.getSensorNumber().is_initialized());
        REQUIRE(*(source.getSensorNumber()) == dummy::getSensor());
    }

    common::OriginalSource source;
    PathTree tree;
};

TEST_CASE_METHOD(PathTreeResolution, "PathTreeResolution-RawAlias") {
    dummy::setupRawAlias(tree);
    checkResolution();
}

TEST_CASE_METHOD(PathTreeResolution, "PathTreeResolution-JSONString") {
    Json::Value val(getFullSourcePath());
    setAlias(val.toStyledString());
    checkResolution();
}

TEST_CASE_METHOD(PathTreeResolution, "PathTreeResolution-ChildString") {
    Json::Value val(Json::objectValue);
    val["child"] = getFullSourcePath();
    setAlias(val.toStyledString());
    checkResolution();
}

TEST_CASE_METHOD(PathTreeResolution, "PathTreeResolution-BackCompatRoute") {
    Json::Value child(Json::objectValue);
    child["tracker"] = dummy::getDevice();
    child["sensor"] = dummy::getSensor();

    Json::Value val(Json::objectValue);
    val["child"] = child;

    setAlias(val.toStyledString());
    checkResolution();
}
