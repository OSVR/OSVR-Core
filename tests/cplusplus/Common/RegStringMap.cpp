/** @file
    @brief Implementation

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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/Common/RegisteredStringMap.h>

// Library/third-party includes
#include <catch2/catch.hpp>

// Standard includes
#include <string>

using osvr::common::CorrelatedStringMap;
using osvr::common::RegisteredStringMap;
using osvr::util::PeerStringID;
using osvr::util::StringID;

TEST_CASE("RegisteredStringMap-create") {

    RegisteredStringMap regMap;
    REQUIRE(0 == regMap.getEntries().size());
    REQUIRE_FALSE(regMap.isModified());
}

TEST_CASE("RegisteredStringMap-addNewValues") {
    RegisteredStringMap regMap;

    StringID regID0 = regMap.registerStringID("RegVal0");
    REQUIRE(regMap.isModified());
    REQUIRE(1 == regMap.getEntries().size());

    StringID regID1 = regMap.registerStringID("RegVal1");
    REQUIRE(regMap.isModified());
    REQUIRE(2 == regMap.getEntries().size());

    StringID regID2 = regMap.registerStringID("RegVal2");
    REQUIRE(regMap.isModified());
    REQUIRE(3 == regMap.getEntries().size());
}

class RegisteredStringMapTest {
  public:
    RegisteredStringMapTest() {
        regID0 = regMap.registerStringID("RegVal0");
        regID1 = regMap.registerStringID("RegVal1");
        regID2 = regMap.registerStringID("RegVal2");

        corID0 = corMap.registerStringID("CorVal0");
        corID1 = corMap.registerStringID("CorVal1");
        corID2 = corMap.registerStringID("CorVal2");
    }

    RegisteredStringMap regMap;
    CorrelatedStringMap corMap;

    StringID regID0, regID1, regID2;
    StringID corID0, corID1, corID2;
};

TEST_CASE_METHOD(RegisteredStringMapTest,
                 "RegisteredStringMapTest-checkExistingValues") {

    regMap.clearModifiedFlag();

    StringID regID = regMap.registerStringID("RegVal0");
    REQUIRE_FALSE(regMap.isModified());
    REQUIRE(3 == regMap.getEntries().size());
    REQUIRE(0 == regID.value());

    regID = regMap.registerStringID("RegVal1");
    REQUIRE_FALSE(regMap.isModified());
    REQUIRE(3 == regMap.getEntries().size());
    REQUIRE(1 == regID.value());

    regID = regMap.registerStringID("RegVal2");
    REQUIRE_FALSE(regMap.isModified());
    REQUIRE(3 == regMap.getEntries().size());
    REQUIRE(2 == regID.value());
}

TEST_CASE_METHOD(RegisteredStringMapTest, "RegisteredStringMapTest-getValues") {

    REQUIRE("RegVal0" == regMap.getStringFromId(regID0));
    REQUIRE("RegVal1" == regMap.getStringFromId(regID1));
    REQUIRE("RegVal2" == regMap.getStringFromId(regID2));
    REQUIRE(regMap.getStringFromId(StringID(1000)).empty());

    REQUIRE("CorVal0" == corMap.getStringFromId(corID0));
    REQUIRE("CorVal1" == corMap.getStringFromId(corID1));
    REQUIRE("CorVal2" == corMap.getStringFromId(corID2));
    REQUIRE(corMap.getStringFromId(StringID(1000)).empty());
}

TEST_CASE_METHOD(RegisteredStringMapTest,
                 "RegisteredStringMapTest-getEntries") {

    auto entries = regMap.getEntries();
    REQUIRE(3 == entries.size());
    REQUIRE("RegVal0" == entries[0]);
    REQUIRE("RegVal1" == entries[1]);
    REQUIRE("RegVal2" == entries[2]);
}

TEST_CASE_METHOD(RegisteredStringMapTest,
                 "RegisteredStringMapTest-checkModified") {
    regMap.clearModifiedFlag();
    REQUIRE_FALSE(regMap.isModified());
}

TEST_CASE_METHOD(RegisteredStringMapTest,
                 "RegisteredStringMapTest-checkOutOfRangePeerID") {

    REQUIRE_THROWS_AS(corMap.convertPeerToLocalID(PeerStringID(100)),
                      std::out_of_range);
}

TEST_CASE_METHOD(RegisteredStringMapTest,
                 "RegisteredStringMapTest-checkEmptyPeerMapping") {

    StringID emptID = corMap.convertPeerToLocalID(PeerStringID());
    REQUIRE(emptID.empty());
}

TEST_CASE_METHOD(RegisteredStringMapTest,
                 "RegisteredStringMapTest-checkSetupPeerMappings") {

    auto entries = regMap.getEntries();
    corMap.setupPeerMappings(entries);

    StringID corID3 = corMap.convertPeerToLocalID(PeerStringID(regID0.value()));
    StringID corID4 = corMap.convertPeerToLocalID(PeerStringID(regID1.value()));
    StringID corID5 = corMap.convertPeerToLocalID(PeerStringID(regID2.value()));

    REQUIRE("RegVal0" == corMap.getStringFromId(corID3));
    REQUIRE("RegVal1" == corMap.getStringFromId(corID4));
    REQUIRE("RegVal2" == corMap.getStringFromId(corID5));
}
