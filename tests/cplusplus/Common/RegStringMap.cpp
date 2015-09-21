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
#include "gtest/gtest.h"

// Standard includes
#include <string>

using osvr::util::StringID;
using osvr::util::PeerStringID;
using osvr::common::RegisteredStringMap;
using osvr::common::CorrelatedStringMap;

class RegisteredStringMapTest : public ::testing::Test {
  public:
    RegisteredStringMapTest() {}

    RegisteredStringMap regMap;
    CorrelatedStringMap corMap;

    StringID regID0, regID1, regID2;
    StringID corID0, corID1, corID2;
};

TEST(RegisteredStringMap, create) {

    RegisteredStringMap regMap;
    ASSERT_EQ(0, regMap.getEntries().size());
    ASSERT_FALSE(regMap.isModified());
}

TEST_F(RegisteredStringMapTest, addNewValues) {

    regID0 = regMap.getStringID("RegVal0");
    ASSERT_TRUE(regMap.isModified());
    ASSERT_EQ(1, regMap.getEntries().size());

    regID1 = regMap.getStringID("RegVal1");
    ASSERT_TRUE(regMap.isModified());
    ASSERT_EQ(2, regMap.getEntries().size());

    regID2 = regMap.getStringID("RegVal2");
    ASSERT_TRUE(regMap.isModified());
    ASSERT_EQ(3, regMap.getEntries().size());
}

TEST_F(RegisteredStringMapTest, checkExistingValues) {

    regMap.clearModifiedFlag();

    StringID regID = regMap.getStringID("RegVal0");
    ASSERT_FALSE(regMap.isModified());
    ASSERT_EQ(3, regMap.getEntries().size());
    ASSERT_EQ(0, regID.value());

    regID = regMap.getStringID("RegVal1");
    ASSERT_FALSE(regMap.isModified());
    ASSERT_EQ(3, regMap.getEntries().size());
    ASSERT_EQ(1, regID.value());

    regID = regMap.getStringID("RegVal2");
    ASSERT_FALSE(regMap.isModified());
    ASSERT_EQ(3, regMap.getEntries().size());
    ASSERT_EQ(2, regID.value());
}

TEST_F(RegisteredStringMapTest, getValues) {

    ASSERT_STREQ("RegVal0", regMap.getStringFromId(regID0).c_str());
    ASSERT_STREQ("RegVal1", regMap.getStringFromId(regID1).c_str());
    ASSERT_STREQ("RegVal2", regMap.getStringFromId(regID2).c_str());
    ASSERT_EQ(0, std::strlen(regMap.getStringFromId(StringID(1000)).c_str()));

    ASSERT_STREQ("CorVal0", corMap.getStringFromId(corID0).c_str());
    ASSERT_STREQ("CorVal1", corMap.getStringFromId(corID1).c_str());
    ASSERT_STREQ("CorVal2", corMap.getStringFromId(corID2).c_str());
    ASSERT_EQ(0, std::strlen(corMap.getStringFromId(StringID(1000)).c_str()));
}

TEST_F(RegisteredStringMapTest, getEntries) {

    auto entries = regMap.getEntries();
    ASSERT_EQ(3, entries.size());
    ASSERT_STREQ("RegVal0", entries[0].c_str());
    ASSERT_STREQ("RegVal1", entries[1].c_str());
    ASSERT_STREQ("RegVal2", entries[2].c_str());
}

TEST_F(RegisteredStringMapTest, checkModified) {
    regMap.clearModifiedFlag();
    ASSERT_FALSE(regMap.isModified());
}

TEST_F(RegisteredStringMapTest, checkEmptyPeerMapping) {

    StringID emptID = corMap.convertPeerToLocalID(PeerStringID(100));
    ASSERT_TRUE(emptID.empty());
}

TEST_F(RegisteredStringMapTest, checkSetupPeerMappings) {

    auto entries = regMap.getEntries();
    corMap.setupPeerMappings(entries);

    StringID corID3 = corMap.convertPeerToLocalID(PeerStringID(regID0.value()));
    StringID corID4 = corMap.convertPeerToLocalID(PeerStringID(regID1.value()));
    StringID corID5 = corMap.convertPeerToLocalID(PeerStringID(regID2.value()));

    ASSERT_STREQ("RegVal0", corMap.getStringFromId(corID3).c_str());
    ASSERT_STREQ("RegVal1", corMap.getStringFromId(corID4).c_str());
    ASSERT_STREQ("RegVal2", corMap.getStringFromId(corID5).c_str());
}
