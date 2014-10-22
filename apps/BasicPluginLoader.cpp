/** @file
    @brief Implementation

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>

*/

// Copyright 2014 Sensics, Inc.
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
// - none

// Library/third-party includes
#include <libfunctionality/LoadPlugin.h>

// Standard includes
#include <iostream>
#include <exception>


int main(int argc, char * argv[]) {
    if (argc < 2) {
        std::cerr << "Must supply a plugin name to load." << std::endl;
        return 1;
    }
    try {
        std::cout << "Trying to load plugin " << argv[1] << std::endl;
        libfunc::PluginHandle plugin = libfunc::loadPluginByName(argv[1], NULL /* context would go here in real loader */);
        std::cout << "Successfully loaded plugin, control returned to host application!" << std::endl;
        return 0;
    } catch (std::exception & e) {
        std::cerr << "Caught exception tring to load " << argv[1] << ": " << e.what() << std::endl;
        return 1;
    }
    std::cerr << "Failed in a weird way - not a std::exception." << std::endl;
    return 2;
}