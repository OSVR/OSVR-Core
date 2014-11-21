/** @file
    @brief Test Implementation

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>

*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include <osvr/Routing/PathTreeFull.h>
#include <osvr/Routing/PathElementTools.h>
#include <osvr/Routing/PathElementTypes.h>
#include <osvr/Routing/PathNode.h>

// Library/third-party includes
#include <boost/variant/get.hpp>
#include "gtest/gtest.h"

// Standard includes
// - none

using std::string;
using namespace osvr::routing;
// using osvr::routing::PathTree;
// namespace elements = osvr::routing::elements;

template <typename ElementType>
inline bool isElementType(elements::PathElement const &elt) {
    return (boost::get<ElementType const>(&elt) != NULL);
}

template <typename ElementType> inline bool isNodeType(PathNode const &node) {
    return isElementType<ElementType>(node.value());
}

TEST(PathTree, create) { ASSERT_NO_THROW(PathTree()); }

TEST(PathElement, getTypeName) {
    ASSERT_STREQ(elements::getTypeName<elements::NullElement>(), "NullElement");
}

TEST(PathElement, isElementType) {
    ASSERT_TRUE(isElementType<elements::NullElement>(elements::NullElement()));
    ASSERT_FALSE(
        isElementType<elements::DeviceElement>(elements::NullElement()));
}

TEST(PathTree, getPathRoot) {
    PathTree tree;
    PathNode *result = NULL;
    ASSERT_NO_THROW(result = &tree.getNodeByPath("/")) << "Get the root.";
    ASSERT_TRUE(result->isRoot());
}

TEST(PathTree, getPathBadInput) {
    PathTree tree;
    ASSERT_THROW(tree.getNodeByPath(""), std::runtime_error)
        << "Empty string not acceptable as path";
    ASSERT_THROW(tree.getNodeByPath("test"), std::runtime_error)
        << "Missing leading slash not acceptable";
}

TEST(PathTree, getPathSingleLevel) {
    PathTree tree;
    PathNode *result = NULL;
    ASSERT_NO_THROW(result = &tree.getNodeByPath("/test"))
        << "Get a new node just a single level in";
    ASSERT_TRUE(isNodeType<elements::NullElement>(*result))
        << "Check the type of the new node.";
    ASSERT_EQ(result->getName(), "test") << "Check the name of the new node";

    ASSERT_NO_THROW(tree.getNodeByPath("/test")) << "Get the same node again.";
    ASSERT_EQ(tree.getNodeByPath("/test"), *result)
        << "Ensure we're actually getting the same identity.";
    ASSERT_EQ(tree.getNodeByPath("/test/"), *result)
        << "Ensure trailing slashes don't matter.";
}

TEST(PathTree, getPathTwoLevel) {
    PathTree tree;
    PathNode *test2 = NULL;
    ASSERT_NO_THROW(test2 = &tree.getNodeByPath("/test1/test2"))
        << "Get a node two new levels in";
    // Check test2
    ASSERT_EQ(test2->getName(), "test2");
    ASSERT_FALSE(test2->hasChildren()) << "Make sure it has no children.";
    ASSERT_TRUE(isNodeType<elements::NullElement>(*test2)) << "Check type";
    ASSERT_FALSE(!test2->getParent()) << "Make sure it has a parent.";

    // Check test1
    PathNodePtr test1 = test2->getParent();
    ASSERT_EQ(test1->getName(), "test1");
    ASSERT_TRUE(isNodeType<elements::NullElement>(*test1));
    ASSERT_FALSE(!test1->getParent()) << "Make sure it has a parent.";
    PathNodePtr root = test1->getParent();

    ASSERT_TRUE(root->isRoot());
    ASSERT_EQ(tree.getNodeByPath("/"), *root)
        << "Root identity should be preserved";
    ASSERT_EQ(tree.getNodeByPath("/test1/test2"), *test2)
        << "Identity should be preserved";
}

TEST(PathTree, addDeviceBadInput) {
    PathTree tree;
    ASSERT_THROW(addDevice(tree, "/org_opengoggles_sample"), std::runtime_error)
        << "Should reject just a single level";
    ASSERT_THROW(addDevice(tree, "/org_opengoggles_sample/"),
                 std::runtime_error)
        << "Should reject just a single level with trailing slash";
    ASSERT_THROW(addDevice(tree, "org_opengoggles_sample"), std::runtime_error)
        << "Should reject just a single level w/o leading slash";
    ASSERT_THROW(addDevice(tree, "org_opengoggles_sample/"), std::runtime_error)
        << "Should reject just a single level with trailing but w/o leading "
           "slash";

    ASSERT_THROW(addDevice(tree, "/org_opengoggles_sample//"),
                 std::runtime_error)
        << "Should reject empty second level";
    ASSERT_THROW(addDevice(tree, "org_opengoggles_sample//"),
                 std::runtime_error)
        << "Should reject empty second level";
    ASSERT_THROW(addDevice(tree, "//"), std::runtime_error);
    ASSERT_THROW(addDevice(tree, "///"), std::runtime_error);
}

TEST(PathTree, addDevice) {
    PathTree tree;
    PathNode *dev = NULL;
    ASSERT_NO_THROW(dev = &addDevice(tree, "/org_opengoggles_sample/MyDevice"));

    // Check Device
    ASSERT_EQ(dev->getName(), "MyDevice");
    ASSERT_FALSE(dev->hasChildren()) << "Make sure it has no children.";
    ASSERT_TRUE(isNodeType<elements::DeviceElement>(*dev)) << "Check type";
    ASSERT_FALSE(!dev->getParent()) << "Make sure it has a parent.";

    // Check org_opengoggles_sample
    PathNodePtr plugin = dev->getParent();
    ASSERT_EQ(plugin->getName(), "org_opengoggles_sample");
    ASSERT_TRUE(isNodeType<elements::PluginElement>(*plugin)) << "Check type";
    ASSERT_FALSE(!plugin->getParent()) << "Make sure it has a parent.";
    PathNodePtr root = plugin->getParent();

    ASSERT_TRUE(root->isRoot());
    ASSERT_EQ(tree.getNodeByPath("/"), *root)
        << "Root identity should be preserved";
    ASSERT_EQ(tree.getNodeByPath("/org_opengoggles_sample/MyDevice"), *dev)
        << "Identity should be preserved";
}

TEST(PathTree, addDeviceMissingLeadingSlash) {
    PathTree tree;

    PathNode *dev = NULL;
    ASSERT_NO_THROW(dev = &addDevice(tree, "org_opengoggles_sample/MyDevice"))
        << "Should forgive a missing leading slash";
    ASSERT_EQ(tree.getNodeByPath("/org_opengoggles_sample/MyDevice"), *dev)
        << "Should be the same as if the slash had been present";
}

TEST(PathTree, getFullPath) {
    PathTree tree;
    ASSERT_EQ(
        getFullPath(tree.getNodeByPath("/org_opengoggles_sample/MyDevice")),
        "/org_opengoggles_sample/MyDevice");
    PathNode *dev = NULL;
    ASSERT_NO_THROW(dev = &addDevice(tree, "/org_opengoggles_sample/MyDevice"));

    // Check Device
    ASSERT_EQ(dev->getName(), "MyDevice");
    ASSERT_FALSE(dev->hasChildren()) << "Make sure it has no children.";
    ASSERT_TRUE(isNodeType<elements::DeviceElement>(*dev)) << "Check type";
    ASSERT_FALSE(!dev->getParent()) << "Make sure it has a parent.";

    // Check org_opengoggles_sample
    PathNodePtr plugin = dev->getParent();
    ASSERT_EQ(plugin->getName(), "org_opengoggles_sample");
    ASSERT_TRUE(isNodeType<elements::PluginElement>(*plugin)) << "Check type";
    ASSERT_FALSE(!plugin->getParent()) << "Make sure it has a parent.";
    PathNodePtr root = plugin->getParent();

    ASSERT_TRUE(root->isRoot());
    ASSERT_EQ(tree.getNodeByPath("/"), *root)
        << "Root identity should be preserved";
    ASSERT_EQ(tree.getNodeByPath("/org_opengoggles_sample/MyDevice"), *dev)
        << "Identity should be preserved";
}