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
#include <osvr/Util/TreeNode.h>

// Library/third-party includes
#include <catch2/catch.hpp>

// Standard includes
#include <string>

using osvr::util::TreeNode;
using osvr::util::TreeNodePointer;
using std::string;

typedef TreeNode<int> IntTree;
typedef TreeNodePointer<int>::type IntTreePtr;

typedef TreeNode<string> StringTree;
typedef TreeNodePointer<string>::type StringTreePtr;

TEST_CASE("TreeNode-createRoot") {
    REQUIRE_NOTHROW((IntTree::createRoot()));
    REQUIRE_NOTHROW((IntTree::createRoot(5)));
    REQUIRE_NOTHROW((StringTree::createRoot()));
    REQUIRE_NOTHROW((StringTree::createRoot("Value")));
}

TEST_CASE("TreeNode-RootInvariants") {
    {
        IntTreePtr tree;
        REQUIRE_NOTHROW(tree = IntTree::createRoot());
        REQUIRE(tree->getName().empty());
        REQUIRE_FALSE(tree->hasChildren());
        REQUIRE(tree->numChildren() == 0);
        {
            INFO("Default constructed int is 0");
            REQUIRE(tree->value() == 0);
        }
        {
            INFO("Can set the value");
            REQUIRE_NOTHROW(tree->value() = 6);
        }
        {
            INFO("Retains set value");
            REQUIRE(tree->value() == 6);
        }
    }
    {
        IntTreePtr tree;
        REQUIRE_NOTHROW(tree = IntTree::createRoot(5));
        REQUIRE(tree->getName().empty());
        REQUIRE_FALSE(tree->hasChildren());
        REQUIRE(tree->numChildren() == 0);
        {
            INFO("We created with value 5");
            REQUIRE(tree->value() == 5);
        }
        {
            INFO("Can set the value");
            REQUIRE_NOTHROW(tree->value() = 6);
        }
        {
            INFO("Retains set value");
            REQUIRE(tree->value() == 6);
        }
    }
    {
        StringTreePtr tree;
        REQUIRE_NOTHROW(tree = StringTree::createRoot());
        REQUIRE(tree->getName().empty());
        REQUIRE_FALSE(tree->hasChildren());
        REQUIRE(tree->numChildren() == 0);
        {
            INFO("Default constructed string is empty");
            REQUIRE(tree->value().empty());
        }
        {
            INFO("Can set the value");
            REQUIRE_NOTHROW(tree->value() = "myVal");
        }
        {
            INFO("Retains set value");
            REQUIRE(tree->value() == "myVal");
        }
    }
    {
        StringTreePtr tree;
        REQUIRE_NOTHROW(tree = StringTree::createRoot("test"));
        REQUIRE(tree->getName().empty());
        REQUIRE_FALSE(tree->hasChildren());
        REQUIRE(tree->numChildren() == 0);
        {
            INFO("Retains original value");
            REQUIRE(tree->value() == "test");
        }
        {
            INFO("Can set the value");
            REQUIRE_NOTHROW(tree->value() = "myVal");
        }
        {
            INFO("Retains set value");
            REQUIRE(tree->value() == "myVal");
        }
    }
}

TEST_CASE("TreeNode-create") {
    IntTreePtr tree(IntTree::createRoot());
    REQUIRE_NOTHROW((IntTree::create(*tree, "A")));
    REQUIRE(tree->hasChildren());
    REQUIRE(tree->numChildren() == 1);
    {
        INFO("Can't create a duplicate-named child");
        REQUIRE_THROWS_AS((IntTree::create(*tree, "A")), std::logic_error);
        REQUIRE(tree->numChildren() == 1);
    }

    {
        INFO("Can't create a non-root node without a name");
        REQUIRE_THROWS_AS((IntTree::create(*tree, "")), std::logic_error);
        REQUIRE(tree->numChildren() == 1);
    }
}

TEST_CASE("TreeNode-getOrCreateChildByName") {
    IntTreePtr tree(IntTree::createRoot());
    REQUIRE_NOTHROW(tree->getOrCreateChildByName("A"));
    REQUIRE(tree->hasChildren());
    REQUIRE(tree->numChildren() == 1);

    REQUIRE_NOTHROW(tree->getOrCreateChildByName("A"));
    REQUIRE(tree->numChildren() == 1);

    {
        INFO("Can't create a non-root node without a name");
        REQUIRE_THROWS_AS(tree->getOrCreateChildByName(""), std::logic_error);
        REQUIRE(tree->numChildren() == 1);
    }
}

TEST_CASE("TreeNode-ChildValues") {
    StringTreePtr tree(StringTree::createRoot());
    {
        INFO("Default constructed string is empty");
        REQUIRE(tree->value().empty());
        REQUIRE(tree->getOrCreateChildByName("A").value().empty());
    }
    REQUIRE(tree->numChildren() == 1);

    {
        INFO("Setting child value");
        REQUIRE_NOTHROW(tree->getOrCreateChildByName("A").value() = "myVal");
        REQUIRE(tree->getOrCreateChildByName("A").value() == "myVal");
    }
    {
        INFO("Other values should still be the same");
        REQUIRE(tree->value().empty());
    }

    REQUIRE_NOTHROW((StringTree::create(*tree, "B")));
    REQUIRE(tree->numChildren() == 2);
    {
        INFO("New child's value is empty");
        REQUIRE(tree->getOrCreateChildByName("B").value().empty());
    }
    {
        INFO("Other values should still be the same");
        REQUIRE(tree->value().empty());
        REQUIRE(tree->getOrCreateChildByName("A").value() == "myVal");
    }
    INFO("Setting second child value");
    REQUIRE_NOTHROW(tree->getOrCreateChildByName("B").value() = "mySecondVal");

    {
        INFO("Other values should still be the same");
        REQUIRE(tree->value().empty());
        REQUIRE(tree->getOrCreateChildByName("A").value() == "myVal");
    }
    REQUIRE(tree->getOrCreateChildByName("B").value() == "mySecondVal");

    INFO("Creating child and setting value in single getOrCreate "
         "expression.");
    REQUIRE_NOTHROW(tree->getOrCreateChildByName("C").value() = "myThirdVal");

    REQUIRE(tree->numChildren() == 3);

    {
        INFO("Other values should still be the same");
        REQUIRE(tree->value().empty());
        REQUIRE(tree->getOrCreateChildByName("A").value() == "myVal");
        REQUIRE(tree->getOrCreateChildByName("B").value() == "mySecondVal");
    }
    REQUIRE(tree->getOrCreateChildByName("C").value() == "myThirdVal");

    INFO("Creating child and setting value in single create() expression.");
    REQUIRE_NOTHROW((StringTree::create(*tree, "D").value() = "myFourthVal"));

    REQUIRE(tree->numChildren() == 4);

    {
        INFO("Other values should still be the same");
        REQUIRE(tree->value().empty());
        REQUIRE(tree->getOrCreateChildByName("A").value() == "myVal");
        REQUIRE(tree->getOrCreateChildByName("B").value() == "mySecondVal");
        REQUIRE(tree->getOrCreateChildByName("C").value() == "myThirdVal");
    }
    REQUIRE(tree->getOrCreateChildByName("D").value() == "myFourthVal");

    INFO("Creating child and setting value in single create() call.");
    REQUIRE_NOTHROW((StringTree::create(*tree, "E", "myFifthVal")));
    REQUIRE(tree->numChildren() == 5);
    {
        INFO("Other values should still be the same");
        REQUIRE(tree->value().empty());
        REQUIRE(tree->getOrCreateChildByName("A").value() == "myVal");
        REQUIRE(tree->getOrCreateChildByName("B").value() == "mySecondVal");
        REQUIRE(tree->getOrCreateChildByName("C").value() == "myThirdVal");
        REQUIRE(tree->getOrCreateChildByName("D").value() == "myFourthVal");
    }
    REQUIRE(tree->getOrCreateChildByName("E").value() == "myFifthVal");
}

class ValueChecker {
  public:
    ValueChecker() : nodes(0) {}
    void operator()(StringTree const &node) {
        {
            INFO("Root is visited first");
            if (nodes == 0) {
                REQUIRE(node.isRoot());
            } else {
                REQUIRE_FALSE(node.isRoot());
            }
        }
        if (node.getName().empty()) {
            INFO("Visiting root");
            REQUIRE(node.isRoot());
            REQUIRE(node.value().empty());
        } else if (node.getName() == "A") {
            INFO("First child's value visited");
            REQUIRE(node.value() == "myVal");
        } else if (node.getName() == "B") {
            INFO("Second child's value visited");
            REQUIRE(node.value() == "mySecondVal");
        } else if (node.getName() == "C") {
            INFO("Third child's value visited");
            REQUIRE(node.value() == "myThirdVal");

        } else if (node.getName() == "D") {
            INFO("Fourth child's value visited");
            REQUIRE(node.value() == "myFourthVal");
        } else if (node.getName() == "E") {
            INFO("Fourth child's value visited");
            REQUIRE(node.value() == "myFifthVal");
        } else {
            INFO("Should only have the root and four child nodes!");
            FAIL();
        }
        nodes++;
    }

    void assertNodeCount() { REQUIRE(nodes == 6); }

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

TEST_CASE("TreeNode-Comparisons") {
    StringTreePtr tree = getFullTree();
    REQUIRE(*tree == *tree);
    REQUIRE_FALSE(*tree == tree->getOrCreateChildByName("A"));
    REQUIRE(tree->getOrCreateChildByName("A") ==
            tree->getOrCreateChildByName("A"));
    REQUIRE_FALSE(tree->getOrCreateChildByName("A") ==
                  tree->getOrCreateChildByName("B"));
}

TEST_CASE("TreeNode-Visitor") {
    StringTreePtr tree = getFullTree();
    ValueVisitor visitor;
    visitor(*tree);
    visitor.checker.assertNodeCount();
}

TEST_CASE("TreeNode-ConstVisitor") {
    StringTreePtr tree = getFullTree();
    ConstValueVisitor visitor;
    visitor(*tree);
    visitor.checker.assertNodeCount();
}

class ParentCheckerVisitor {
  public:
    ParentCheckerVisitor() : nodes(0), root(nullptr) {}
    void operator()(StringTree const &node) {
        if (nodes == 0) {
            // root
            REQUIRE(node.isRoot());
            REQUIRE(node.getParent() == nullptr);
            root = &node;
        } else {
            // not root
            REQUIRE_FALSE(node.isRoot());
            REQUIRE(node.getParent() == root);
        }
        nodes++;
        node.visitConstChildren(*this);
    }
    size_t nodes;
    StringTree const *root;
};

TEST_CASE("TreeNode-ParentPointers") {
    StringTreePtr tree = getFullTree();
    ParentCheckerVisitor visitor;
    visitor(*tree);
}
