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
#include "GraphOutputInterface.h"

// Library/third-party includes
#include <boost/program_options.hpp>

// Standard includes
#include <iostream>
#include <fstream>

int osvrToStream(std::ostream &os, std::string const& type) {
    osvr::clientkit::ClientContext context("com.osvr.osvr2dot");
    auto const &pathTree = context.get()->getPathTree();
    {
        auto graph = GraphOutputInterface::createGraphOutputInterface(os, type);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    std::string type;
    {
        namespace po = boost::program_options;
        // clang-format off
        po::options_description desc("Options");
        desc.add_options()
            ("help", "produce help message")
            ("output,O", po::value<std::string>(), "output file (defaults to standard out)")
            ("type,T", po::value<std::string>(&type)->default_value("dot"), "output data format (defaults to graphviz/dot)")
            ;
        // clang-format on
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
        po::notify(vm);
        if (vm.count("help")) {
            std::cerr << "Usage: " << argv[0] << " [options]\n";
            std::cerr << desc << "\n";
            return 1;
        }
        if (vm.count("output")) {
            std::ofstream file(vm["output"].as<std::string>().c_str());
            if (!file) {
                std::cerr << "Error opening output file." << std::endl;
                return 1;
            }
            return osvrToStream(file, type);
        }
        return osvrToStream(std::cout, type);
    }
}