/** @file
    @brief Implementation

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>

*/

// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include <osvr/ClientKit/ClientKit.h>
#include <osvr/Client/ClientContext.h>
#include <osvr/Util/EigenInterop.h>
#include <osvr/Util/UniquePtr.h>

// Library/third-party includes
#include <boost/program_options.hpp>
#include <json/value.h>
#include <json/reader.h>

// Standard includes
#include <iostream>
#include <fstream>
#include <exception>

using std::cout;
using std::cerr;
using std::endl;

int main(int argc, char *argv[]) {
    namespace po = boost::program_options;
    // clang-format off
    po::options_description desc("Options");
    desc.add_options()
        ("help", "produce help message")
        ("route", po::value<std::string>()->default_value("/me/head"), "route to calibrate")
        ;
    // clang-format on

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
    po::notify(vm);

    {
        /// Deal with command line errors or requests for help
        bool usage = false;

        if (vm.count("help")) {
            cout << "Usage: osvr_reset_yaw [options]" << endl;
            cout << desc << "\n";
            return 1;
        }
    }
    osvr::clientkit::ClientContext ctx("com.osvr.bundled.resetyaw");

    return 0;
}