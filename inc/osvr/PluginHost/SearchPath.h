/** @file
    @brief Header

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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_SearchPath_h_GUID_D9D19BF6_FEB5_4B82_17A4_C8C397C88523
#define INCLUDED_SearchPath_h_GUID_D9D19BF6_FEB5_4B82_17A4_C8C397C88523

// Internal Includes
#include <osvr/PluginHost/Export.h>

// Library/third-party includes
// - none

// Standard includes
#include <vector>
#include <string>

namespace osvr {
namespace pluginhost {

    typedef std::vector<std::string> SearchPath;
    typedef std::vector<std::string> FileList;

    /// Find a place where to look for plugins
    OSVR_PLUGINHOST_EXPORT SearchPath getPluginSearchPath();

    /// Get list of files inside the directory with given extension
    OSVR_PLUGINHOST_EXPORT FileList
    getAllFilesWithExt(SearchPath const &dirPath, const std::string &ext);

    /// Given the name of a plugin, find the full path to the plugin library.
    OSVR_PLUGINHOST_EXPORT std::string
    findPlugin(SearchPath const &searchPaths, const std::string &pluginName);

} // namespace pluginhost
} // namespace osvr

#endif // INCLUDED_SearchPath_h_GUID_D9D19BF6_FEB5_4B82_17A4_C8C397C88523
