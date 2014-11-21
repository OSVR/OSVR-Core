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
#include <osvr/Util/TreeNode.h>

// Library/third-party includes
#include "gtest/gtest.h"

// Standard includes
#include <string>

using std::string;
using osvr::util::TreeNode;
using osvr::util::TreeNodePointer;

typedef TreeNode<int> IntTree;
typedef TreeNodePointer<int>::type IntTreePtr;

typedef TreeNode<string> StringTree;
typedef TreeNodePointer<string>::type StringTreePtr;

TEST(TreeNode, createRoot) {
    ASSERT_NO_THROW((IntTree::createRoot()));
    ASSERT_NO_THROW((IntTree::createRoot(5)));
    ASSERT_NO_THROW((StringTree::createRoot()));
    ASSERT_NO_THROW((StringTree::createRoot("Value")));
}

TEST(TreeNode, RootInvariants) {
    {
        IntTreePtr tree;
        ASSERT_NO_THROW(tree = IntTree::createRoot());
        ASSERT_TRUE(tree->getName().empty());
        ASSERT_FALSE(tree->hasChildren());
        ASSERT_EQ(tree->numChildren(), 0);
        ASSERT_EQ(tree->value(), 0) << "Default constructed int is 0";
        ASSERT_NO_THROW(tree->value() = 6) << "Can set the value";
        ASSERT_EQ(tree->value(), 6) << "Retains set value";
    }
    {
        IntTreePtr tree;
        ASSERT_NO_THROW(tree = IntTree::createRoot(5));
        ASSERT_TRUE(tree->getName().empty());
        ASSERT_FALSE(tree->hasChildren());
        ASSERT_EQ(tree->numChildren(), 0);
        ASSERT_EQ(tree->value(), 5) << "We created with value 5";
        ASSERT_NO_THROW(tree->value() = 6) << "Can set the value";
        ASSERT_EQ(tree->value(), 6) << "Retains set value";
    }
    {
        StringTreePtr tree;
        ASSERT_NO_THROW(tree = StringTree::createRoot());
        ASSERT_TRUE(tree->getName().empty());
        ASSERT_FALSE(tree->hasChildren());
        ASSERT_EQ(tree->numChildren(), 0);
        ASSERT_TRUE(tree->value().empty())
            << "Default constructed string is empty";
        ASSERT_NO_THROW(tree->value() = "myVal") << "Can set the value";
        ASSERT_EQ(tree->value(), "myVal") << "Retains set value";
    }
    {
        StringTreePtr tree;
        ASSERT_NO_THROW(tree = StringTree::createRoot("test"));
        ASSERT_TRUE(tree->getName().empty());
        ASSERT_FALSE(tree->hasChildren());
        ASSERT_EQ(tree->numChildren(), 0);
        ASSERT_EQ(tree->value(), "test") << "Retains original value";
        ASSERT_NO_THROW(tree->value() = "myVal") << "Can set the value";
        ASSERT_EQ(tree->value(), "myVal") << "Retains set value";
    }
}

TEST(TreeNode, create) {
    IntTreePtr tree(IntTree::createRoot());
    ASSERT_NO_THROW((IntTree::create(*tree, "A")));
    ASSERT_TRUE(tree->hasChildren());
    ASSERT_EQ(tree->numChildren(), 1);

    ASSERT_THROW((IntTree::create(*tree, "A")), std::logic_error)
        << "Can't create a duplicate-named child";
    ASSERT_EQ(tree->numChildren(), 1);

    ASSERT_THROW((IntTree::create(*tree, "")), std::logic_error)
        << "Can't create a non-root node without a name";
    ASSERT_EQ(tree->numChildren(), 1);
}

TEST(TreeNode, getOrCreateChildByName) {
    IntTreePtr tree(IntTree::createRoot());
    ASSERT_NO_THROW(tree->getOrCreateChildByName("A"))
        << "Should create the child";
    ASSERT_TRUE(tree->hasChildren());
    ASSERT_EQ(tree->numChildren(), 1);

    ASSERT_NO_THROW(tree->getOrCreateChildByName("A"))
        << "Should just retrieve the child";
    ASSERT_EQ(tree->numChildren(), 1) << "Should just retrieve the child";

    ASSERT_THROW(tree->getOrCreateChildByName(""), std::logic_error)
        << "Can't create a non-root node without a name";
    ASSERT_EQ(tree->numChildren(), 1);
}

TEST(TreeNode, ChildValues) {
    StringTreePtr tree(StringTree::createRoot());
    ASSERT_TRUE(tree->value().empty()) << "Default constructed string is empty";
    ASSERT_TRUE(tree->getOrCreateChildByName("A").value().empty())
        << "Default constructed string is empty";
    ASSERT_EQ(tree->numChildren(), 1);

    ASSERT_NO_THROW(tree->getOrCreateChildByName("A").value() = "myVal")
        << "Setting child value";
    ASSERT_TRUE(tree->value().empty())
        << "Parent value should still be default";
    ASSERT_EQ(tree->getOrCreateChildByName("A").value(), "myVal")
        << "Child value retained";

    ASSERT_NO_THROW((StringTree::create(*tree, "B")));
    ASSERT_EQ(tree->numChildren(), 2);
    ASSERT_TRUE(tree->getOrCreateChildByName("B").value().empty())
        << "Second child's value is empty";
    ASSERT_TRUE(tree->value().empty())
        << "Parent value should still be default";
    ASSERT_EQ(tree->getOrCreateChildByName("A").value(), "myVal")
        << "First child value retained";

    ASSERT_NO_THROW(tree->getOrCreateChildByName("B").value() = "mySecondVal")
        << "Setting second child value";
    ASSERT_TRUE(tree->value().empty())
        << "Parent value should still be default";
    ASSERT_EQ(tree->getOrCreateChildByName("A").value(), "myVal")
        << "First child value retained";
    ASSERT_EQ(tree->getOrCreateChildByName("B").value(), "mySecondVal")
        << "Second child's value retained";

    ASSERT_NO_THROW(tree->getOrCreateChildByName("C").value() = "myThirdVal")
        << "Creating child and setting value in single getOrCreate expression.";
    ASSERT_EQ(tree->numChildren(), 3);
    ASSERT_TRUE(tree->value().empty())
        << "Parent value should still be default";
    ASSERT_EQ(tree->getOrCreateChildByName("A").value(), "myVal")
        << "First child value retained";
    ASSERT_EQ(tree->getOrCreateChildByName("B").value(), "mySecondVal")
        << "Second child's value retained";
    ASSERT_EQ(tree->getOrCreateChildByName("C").value(), "myThirdVal")
        << "Third child's value retained";

    ASSERT_NO_THROW((StringTree::create(*tree, "D").value() = "myFourthVal"))
        << "Creating child and setting value in single create() expression.";
    ASSERT_EQ(tree->numChildren(), 4);
    ASSERT_TRUE(tree->value().empty())
        << "Parent value should still be default";
    ASSERT_EQ(tree->getOrCreateChildByName("A").value(), "myVal")
        << "First child value retained";
    ASSERT_EQ(tree->getOrCreateChildByName("B").value(), "mySecondVal")
        << "Second child's value retained";
    ASSERT_EQ(tree->getOrCreateChildByName("C").value(), "myThirdVal")
        << "Third child's value retained";
    ASSERT_EQ(tree->getOrCreateChildByName("D").value(), "myFourthVal")
        << "Fourth child's value retained";

    ASSERT_NO_THROW((StringTree::create(*tree, "E", "myFifthVal")))
        << "Creating child and setting value in single create() call.";
    ASSERT_EQ(tree->numChildren(), 5);
    ASSERT_TRUE(tree->value().empty())
        << "Parent value should still be default";
    ASSERT_EQ(tree->getOrCreateChildByName("A").value(), "myVal")
        << "First child value retained";
    ASSERT_EQ(tree->getOrCreateChildByName("B").value(), "mySecondVal")
        << "Second child's value retained";
    ASSERT_EQ(tree->getOrCreateChildByName("C").value(), "myThirdVal")
        << "Third child's value retained";
    ASSERT_EQ(tree->getOrCreateChildByName("D").value(), "myFourthVal")
        << "Fourth child's value retained";
    ASSERT_EQ(tree->getOrCreateChildByName("E").value(), "myFifthVal")
        << "Fifth child's value retained";
}

class ValueChecker {
  public:
    ValueChecker() : nodes(0) {}
    void operator()(StringTree const &node) {
        ASSERT_EQ(nodes == 0, node.isRoot()) << "Root is visited first";
        if (node.getName().empty()) {
            ASSERT_TRUE(node.isRoot()) << "Root has no name.";
            ASSERT_TRUE(node.value().empty()) << "Root has no value set.";
        } else if (node.getName() == "A") {
            ASSERT_EQ(node.value(), "myVal") << "First child value visited";
        } else if (node.getName() == "B") {
            ASSERT_EQ(node.value(), "mySecondVal")
                << "Second child's value visited";
        } else if (node.getName() == "C") {
            ASSERT_EQ(node.value(), "myThirdVal")
                << "Third child's value visited";
        } else if (node.getName() == "D") {
            ASSERT_EQ(node.value(), "myFourthVal")
                << "Fourth child's value visited";
        } else if (node.getName() == "E") {
            ASSERT_EQ(node.value(), "myFifthVal")
                << "Fourth child's value visited";
        } else {
            FAIL() << "Should only have the root and four child nodes!";
        }
        nodes++;
    }

    void assertNodeCount() { ASSERT_EQ(nodes, 6); }

  private:
    size_t nodes;
};

class ValueVisitor {
  public:
    void operator()(StringTree &node) {
        checker(node);
        node.visitChildren(*this);
    }
    ValueChecker checker;
};

class ConstValueVisitor {
  public:
    void operator()(StringTree const &node) {
        checker(node);
        node.visitConstChildren(*this);
    }
    ValueChecker checker;
};

StringTreePtr getFullTree() {
    // Setup tree in all the different ways possible as above.
    StringTreePtr tree(StringTree::createRoot());
    tree->getOrCreateChildByName("A");
    tree->getOrCreateChildByName("A").value() = "myVal";
    StringTree::create(*tree, "B");
    tree->getOrCreateChildByName("B").value() = "mySecondVal";
    tree->getOrCreateChildByName("C").value() = "myThirdVal";
    StringTree::create(*tree, "D").value() = "myFourthVal";
    StringTree::create(*tree, "E", "myFifthVal");
    return tree;
}

TEST(TreeNode, Comparisons) {
    StringTreePtr tree = getFullTree();
    ASSERT_EQ(*tree, *tree);
    ASSERT_NE(*tree, tree->getOrCreateChildByName("A"));
    ASSERT_EQ(tree->getOrCreateChildByName("A"),
              tree->getOrCreateChildByName("A"));
    ASSERT_NE(tree->getOrCreateChildByName("A"),
              tree->getOrCreateChildByName("B"));
}

TEST(TreeNode, Visitor) {
    StringTreePtr tree = getFullTree();
    ValueVisitor visitor;
    visitor(*tree);
    visitor.checker.assertNodeCount();
}

TEST(TreeNode, ConstVisitor) {
    StringTreePtr tree = getFullTree();
    ConstValueVisitor visitor;
    visitor(*tree);
    visitor.checker.assertNodeCount();
}

class ParentCheckerVisitor {
  public:
    ParentCheckerVisitor() : nodes(0), root(NULL) {}
    void operator()(StringTree const &node) {
        if (nodes == 0) {
            // root
            ASSERT_TRUE(node.isRoot());
            ASSERT_EQ(node.getParent(), nullptr);
            root = &node;
        } else {
            // not root
            ASSERT_FALSE(node.isRoot());
            ASSERT_EQ(node.getParent(), root);
        }
        nodes++;
        node.visitConstChildren(*this);
    }
    size_t nodes;
    StringTree const *root;
};

TEST(TreeNode, ParentPointers) {
    StringTreePtr tree = getFullTree();
    ParentCheckerVisitor visitor;
    visitor(*tree);
}