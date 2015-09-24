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
#include <osvr/Common/PathTreeSerialization.h>

// Library/third-party includes
#include <boost/program_options.hpp>
#include <json/value.h>

// Standard includes
#include <iostream>
#include <thread>
#include <chrono>

int main(int argc, char *argv[]) {
    namespace po = boost::program_options;
    // clang-format off
    po::options_description desc("Options");
    desc.add_options()
        ("help,h", "produce help message")
        ;
    // clang-format on
    po::variables_map vm;
    bool usage = false;
    try {
        po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
        po::notify(vm);
    } catch (std::exception &e) {
        std::cerr << "\nError parsing command line: " << e.what() << "\n\n";
        usage = true;
    }
    if (usage || vm.count("help")) {
        std::cerr << "\n Dumps the serialized JSON format of the path tree.\n";
        std::cerr << "Usage: " << argv[0] << "\n\n";
        return 1;
    }

    {
        /// We only actually need the client open for long enough to get the
        /// path tree and serialize it.
        osvr::clientkit::ClientContext context("com.osvr.tools.dumptreejson");

        if (!context.checkStatus()) {
            std::cerr << "Client context has not yet started up - waiting. "
                         "Make sure the server is running."
                      << std::endl;
            do {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                context.update();
            } while (!context.checkStatus());
            std::cerr << "OK, client context ready. Proceeding." << std::endl;
        }

        /// Serialize
        auto nodes = osvr::common::pathTreeToJson(context.get()->getPathTree());
        std::cout << nodes.toStyledString() << std::endl;
    }
    return 0;
}
