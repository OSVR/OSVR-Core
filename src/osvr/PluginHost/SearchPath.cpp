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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/PluginHost/SearchPath.h>
#include <osvr/PluginHost/PathConfig.h>
#include "BinaryLocation.h"
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

// Standard includes
#include <vector>

namespace osvr {
namespace pluginhost {
    SearchPath getPluginSearchPath() {
        using boost::filesystem::path;
        auto exeLocation = path{getBinaryLocation()};
#ifdef __ANDROID__
        OSVR_DEV_VERBOSE("Binary location: " << exeLocation);
#endif
        // Should be something like PREFIX/bin or PREFIX/bin/Release (depending
        // on if we're installed or in a build tree)
        auto binDir = exeLocation.parent_path();
#ifdef _MSC_VER
        /// CMAKE_INTDIR is a string like "Debug", defined automatically
        if (binDir.filename() == CMAKE_INTDIR) {
            binDir = binDir.parent_path();
        }
#endif

        // binDir now normalized to PREFIX/bin

        SearchPath paths;

#ifdef OSVR_PLUGINS_UNDER_BINDIR
        // If the plugin directory is a subdirectory of where the binaries are,
        // no need to go up a level before searching.
        paths.push_back((binDir / OSVR_PLUGIN_SUBDIR).string());
#endif

        // For each component of the compiled-in binary directory, we lop off a
        // component from the binary location we've detected, and add it to the
        // list of possible roots.
        std::vector<path> rootDirCandidates;
        {
            auto compiledBinDir = path{OSVR_BINDIR};
            auto root = binDir.parent_path();
            do {
                rootDirCandidates.push_back(root);
                root = root.parent_path();
                compiledBinDir = compiledBinDir.parent_path();
            } while (!compiledBinDir.empty() && !root.empty());
        }

#ifdef __ANDROID__
        // current working directory, if different from root
        auto currentWorkingDirectory = boost::filesystem::current_path();
        if (currentWorkingDirectory != root) {
            paths.push_back(
                (currentWorkingDirectory / OSVR_PLUGIN_DIR).string());
        }
#endif

        for (auto const &possibleRoot : rootDirCandidates) {
#ifdef _MSC_VER
            paths.push_back(
                (possibleRoot / OSVR_PLUGIN_DIR / CMAKE_INTDIR).string());
#endif
            paths.push_back((possibleRoot / OSVR_PLUGIN_DIR).string());
        }

        /// @todo add user's home directory to search path

        return paths;
    }

    FileList getAllFilesWithExt(SearchPath dirPath, const std::string &ext) {
        FileList filesPaths;

        for (const auto &path : dirPath) {
            using boost::filesystem::directory_iterator;
            using boost::make_iterator_range;
            boost::filesystem::path directoryPath(path);

            // Make sure that directory exists
            if (!boost::filesystem::exists(directoryPath)) {
                continue;
            }

            // Get a list of files inside the dir that match the extension
            for (const auto &pathName : make_iterator_range(
                     directory_iterator(directoryPath), directory_iterator())) {
                if (!boost::filesystem::is_regular_file(pathName) ||
                    pathName.path().extension() != ext)
                    continue;

                filesPaths.push_back(pathName.path().generic_string());
            }
        }

        return filesPaths;
    }

    std::string findPlugin(const std::string &pluginName) {
        auto searchPaths = getPluginSearchPath();
        for (const auto &searchPath : searchPaths) {
            if (!boost::filesystem::exists(searchPath))
                continue;

            using boost::filesystem::directory_iterator;
            using boost::make_iterator_range;

            for (const auto &pluginPathName : make_iterator_range(
                     directory_iterator(searchPath), directory_iterator())) {
                /// Must be a regular file
                /// @todo does this mean symlinks get excluded?
                if (!boost::filesystem::is_regular_file(pluginPathName))
                    continue;

                const auto pluginCandidate =
                    boost::filesystem::path(pluginPathName);

                /// Needs right extension
                if (pluginCandidate.extension().generic_string() !=
                    OSVR_PLUGIN_EXTENSION) {
                    continue;
                }
                const auto pluginBaseName =
                    pluginCandidate.filename().stem().generic_string();
                /// If the name is right or has the manual load suffix, this is
                /// a good one.
                if ((pluginBaseName == pluginName) ||
                    (pluginBaseName ==
                     pluginName + OSVR_PLUGIN_IGNORE_SUFFIX)) {
                    return pluginPathName.path().generic_string();
                }
            }
        }

        return std::string();
    }

} // namespace pluginhost
} // namespace osvr
