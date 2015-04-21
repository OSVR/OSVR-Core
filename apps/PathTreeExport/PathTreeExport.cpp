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
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/ClientKit/Context.h>
#include <osvr/Common/ClientContext.h>
#include <osvr/Common/PathTreeFull.h>
#include <osvr/Common/PathNode.h>
#include <osvr/Common/ResolveTreeNode.h>
#include <osvr/Util/TreeTraversalVisitor.h>
#include <osvr/Common/PathElementTypes.h>
#include <osvr/Common/PathElementTools.h>
#include <osvr/Common/ParseAlias.h>
#include <osvr/Util/Verbosity.h>
#include "GraphOutputInterface.h"

// Library/third-party includes
#include <boost/program_options.hpp>
#include <boost/variant.hpp>

// Standard includes
#include <iostream>
#include <fstream>

struct Options {
    std::string graphOutputType = "dot";
    bool fullPaths = false;
    bool showAliases = false;
    bool showTree = true;
};
int osvrToStream(std::ostream &os, Options const &opts) {
    osvr::clientkit::ClientContext context("com.osvr.tools.pathtreexport");

    /// Get a non-const copy of the path tree.
    osvr::common::PathTree pathTree;
    osvr::common::clonePathTree(context.get()->getPathTree(), pathTree);
    {
        auto graph = GraphOutputInterface::createGraphOutputInterface(
            os, opts.graphOutputType);
        using namespace osvr::common;
        /// First traverse to ensure all aliases are resolved
        osvr::util::traverseWith(
            pathTree.getRoot(),
            [&pathTree](osvr::common::PathNode const &node) {
                resolveTreeNode(pathTree, getFullPath(node));
            });

        /// Now traverse for tree node output
        bool fullPaths = opts.fullPaths;
        osvr::util::traverseWith(
            pathTree.getRoot(),
            [&graph, &pathTree, fullPaths](osvr::common::PathNode const &node) {
                auto parent = node.getParent();
                if (nullptr == parent) {
                    return;
                }
                auto fullPath = getFullPath(node);
                auto &graphNode = graph->addNode(
                    fullPaths ? fullPath : ("/" + node.getName()), fullPath,
                    getTypeName(node));
            });

        if (opts.showTree) {
            /// Now traverse for parent link output
            graph->enableTreeOrganization();
            osvr::util::traverseWith(
                pathTree.getRoot(), [&graph, &pathTree, fullPaths](
                                        osvr::common::PathNode const &node) {
                    auto parent = node.getParent();
                    if (nullptr == parent) {
                        return;
                    }
                    auto fullPath = getFullPath(node);
                    auto &graphNode = graph->getNode(getFullPath(node));
                    graph->addEdge(graph->getNode(getFullPath(*parent)),
                                   graphNode, "child");
                });
        }
        if (opts.showAliases) {
            /// Now traverse for aliases
            osvr::util::traverseWith(
                pathTree.getRoot(),
                [&graph, &pathTree](osvr::common::PathNode const &node) {
                    auto alias =
                        boost::get<elements::AliasElement>(&node.value());
                    if (nullptr == alias) {
                        return;
                    }
                    auto fullPath = getFullPath(node);
                    ParsedAlias parsed(alias->getSource());
                    auto &graphNode = graph->getNode(fullPath);

                    PathNode &source = pathTree.getNodeByPath(parsed.getLeaf());
                    graph->addEdge(
                        graphNode, graph->getNode(getFullPath(source)), "alias",
                        parsed.isSimple() ? std::string() : parsed.getAlias());
                });
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    std::ostream *stream = &std::cout;
    Options opt;
    std::string type;
    {
        namespace po = boost::program_options;
        // clang-format off
        po::options_description desc("Options");
        desc.add_options()
            ("help,h", "produce help message")
            ("output,O", po::value<std::string>(), "output file (defaults to standard out)")
            //("type,T", po::value<std::string>(&opt.graphOutputType)->default_value("dot"), "output data format (defaults to graphviz/dot)")
            ("show-tree,t", po::value<bool>(&opt.showTree)->default_value(true), "Whether or not to show the path tree structure")
            ("show-aliases,a", po::value<bool>(&opt.showAliases)->default_value(false), "Whether or not to show the alias links")
            ("full-paths,p", po::value<bool>(&opt.fullPaths)->default_value(false), "Whether or not to use a node's full path as its label")
            ;
        // clang-format on
        po::variables_map vm;
        bool usage = false;
        try {
            po::store(po::command_line_parser(argc, argv).options(desc).run(),
                      vm);
            po::notify(vm);
        } catch (std::exception &e) {
            std::cerr << "\nError parsing command line: " << e.what()
                      << std::endl;
            usage = true;
        }
        if (usage || vm.count("help")) {
            std::cerr
                << "\nTraverses the path tree and outputs it as input data "
                   "for a graph software package\n";
            std::cerr << "Usage: " << argv[0] << " [options]\n";
            std::cerr << desc << "\n";
            return 1;
        }
        std::ofstream file;
        if (vm.count("output")) {
            file.open(vm["output"].as<std::string>().c_str());
            if (!file) {
                std::cerr << "Error opening output file." << std::endl;
                return 1;
            }
            stream = &file;
        }
        return osvrToStream(*stream, opt);
    }
}