/** @file
    @brief Helper functions for determine where to load or store configuration files, log files, etc.

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com>

*/

// Copyright 2016 Sensics, Inc.
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

#ifndef INCLUDED_Directories_h_GUID_924A614D_303C_470D_9AC4_01BCBE8F1104
#define INCLUDED_Directories_h_GUID_924A614D_303C_470D_9AC4_01BCBE8F1104

// Internal Includes
#include <osvr/Util/Export.h>

// Library/third-party includes
// - none

// Standard includes
#include <string>
#include <vector>

namespace osvr {
namespace util {

enum OSVR_UTIL_EXPORT DirectoryType {
    USER_DIRECTORY_DATA,         ///< user-specific data files
    USER_DIRECTORY_CONFIG,       ///< user-specific configuration files
    USER_DIRECTORY_CACHE,        ///< user-specific non-essential data files
    USER_DIRECTORY_STATE,        ///< user-specific state files
    USER_DIRECTORY_LOG,          ///< user-specific log files
    SITE_DIRECTORY_DATA,         ///< system-wide data files
    SITE_DIRECTORY_CONFIG,       ///< system-wide configuration files
    USER_DIRECTORY_DESKTOP,      ///< the user's Desktop directory
    USER_DIRECTORY_DOCUMENTS,    ///< the user's Documents directory
    USER_DIRECTORY_DOWNLOAD,     ///< the user's Downloads directory
    USER_DIRECTORY_MUSIC,        ///< the user's Music directory
    USER_DIRECTORY_PICTURES,     ///< the user's Pictures directory
    USER_DIRECTORY_PUBLIC_SHARE, ///< the user's shared directory
    USER_DIRECTORY_TEMPLATES,    ///< the user's Templates directory
    USER_DIRECTORY_VIDEOS        ///< the user's Movies directory
};

OSVR_UTIL_EXPORT std::vector<std::string> getDirectories(DirectoryType dirtype, const std::string& appname = "OSVR", const std::string& author = "", const std::string& version = "");

/**
 * @brief Return full path to the requested directory type.
 *
 * @param dirtype The requested directory to return.
 */
OSVR_UTIL_EXPORT std::string getDirectory(const DirectoryType dirtype, const std::string& appname = "OSVR", const std::string& author = "", const std::string& version = "");

OSVR_UTIL_EXPORT std::string getUserDataDir(const std::string& appname = "OSVR", const std::string& author = "", const std::string& version = "");

} // end namespace util
} // end namespace osvr

#endif // INCLUDED_Directories_h_GUID_924A614D_303C_470D_9AC4_01BCBE8F1104

