/** @file
    @brief Test Implementation

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>

*/

// Copyright 2016 Sensics, Inc.
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
#include <osvr/Util/Directories.h>
#include <osvr/Util/PlatformConfig.h>

// Library/third-party includes
#include "gtest/gtest.h"

// Standard includes
#include <string>

using std::string;

#define print_dir(x)                                                           \
    std::cout << #x << " = " << osvr::util::getDirectory(x) << std::endl;

TEST(Directories, Directories) {
#if defined(OSVR_WINDOWS)
    std::cout << "Platform: Windows" << std::endl;
#elif defined(OSVR_MACOSX)
    std::cout << "Platform: macOS" << std::endl;
#elif defined(OSVR_LINUX)
    std::cout << "Platform: Linux" << std::endl;
#else
    std::cout << " -- Unsupported platform!" << std::endl;
#endif

    using namespace osvr::util;

    print_dir(USER_DIRECTORY_DATA);
    print_dir(USER_DIRECTORY_CONFIG);
    print_dir(USER_DIRECTORY_CACHE);
    print_dir(USER_DIRECTORY_STATE);
    print_dir(USER_DIRECTORY_LOG);
    print_dir(SITE_DIRECTORY_DATA);
    print_dir(SITE_DIRECTORY_CONFIG);
    print_dir(USER_DIRECTORY_DESKTOP);
    print_dir(USER_DIRECTORY_DOCUMENTS);
    print_dir(USER_DIRECTORY_DOWNLOAD);
    print_dir(USER_DIRECTORY_MUSIC);
    print_dir(USER_DIRECTORY_PICTURES);
    print_dir(USER_DIRECTORY_PUBLIC_SHARE);
    print_dir(USER_DIRECTORY_TEMPLATES);
    print_dir(USER_DIRECTORY_VIDEOS);
}

