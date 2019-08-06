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
#include <osvr/PluginHost/PathConfig.h>
#include <osvr/PluginHost/SearchPath.h>
#include <osvr/Util/BinaryLocation.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

// Standard includes
#include <algorithm>
#include <string>
#include <vector>

namespace osvr {
namespace pluginhost {

    using boost::filesystem::directory_iterator;
    using boost::make_iterator_range;
    namespace fs = boost::filesystem;

    SearchPath getPluginSearchPath() {
        auto exeLocation = fs::path{util::getBinaryLocation()};
#ifdef __ANDROID__
        OSVR_DEV_VERBOSE("Binary location: " << exeLocation);
#endif
        // Should be something like PREFIX/bin or PREFIX/bin/Release (depending
        // on if we're installed or in a build tree)
        auto binDir = exeLocation.parent_path();

#if defined(_MSC_VER) && defined(CMAKE_INTDIR)
        /// CMAKE_INTDIR is a string like "Debug", defined automatically if
        /// building in some multi-config generator mode of using the MSVC
        /// compiler (the IDE or MSBuild on a solution).
        /// Not defined in makefiles (nmake, jom, ninja), though, hence why we
        /// have to check both definitions.
        if (binDir.filename() == CMAKE_INTDIR) {
            binDir = binDir.parent_path();
        }
#endif

        // binDir now normalized to PREFIX/bin

        /// This will become our return value.
        SearchPath paths;

        // Lambda to add a path, if it's not already in the list.
        auto addUniquePath = [&paths](fs::path const &path) {
            // OSVR_DEV_VERBOSE("Adding search path " << path);
            auto pathString = path.generic_string();
            if (std::find(begin(paths), end(paths), pathString) == end(paths)) {
                // if we didn't already add this path, add it now.
                paths.emplace_back(std::move(pathString));
            }
        };

#ifdef OSVR_PLUGINS_UNDER_BINDIR
        // If the plugin directory is a subdirectory of where the binaries are,
        // no need to go up a level before searching.
        addUniquePath(binDir / OSVR_PLUGIN_SUBDIR);
#endif

        // For each component of the compiled-in binary directory, we lop off a
        // component from the binary location we've detected, and add it to the
        // list of possible roots.
        auto root = binDir.parent_path();
        std::vector<fs::path> rootDirCandidates;
        {
            auto compiledBinDir = fs::path{OSVR_BINDIR};
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
            addUniquePath(currentWorkingDirectory / OSVR_PLUGIN_DIR);
        }

#endif

        for (auto const &possibleRoot : rootDirCandidates) {

#if defined(_MSC_VER) && defined(CMAKE_INTDIR)
            addUniquePath(possibleRoot / OSVR_PLUGIN_DIR / CMAKE_INTDIR);
#endif
            addUniquePath(possibleRoot / OSVR_PLUGIN_DIR);
        }

        /// @todo add user's home directory to search path

        return paths;
    }

    FileList getAllFilesWithExt(SearchPath const &dirPath,
                                const std::string &ext) {
        FileList filesPaths;

        for (const auto &path : dirPath) {
            fs::path directoryPath(path);

            // Make sure that directory exists
            if (!fs::exists(directoryPath)) {
                continue;
            }

            // Get a list of files inside the dir that match the extension
            for (const auto &pathName : make_iterator_range(
                     directory_iterator(directoryPath), directory_iterator())) {
                if (!fs::is_regular_file(pathName) ||
                    pathName.path().extension() != ext) {
                    continue;
                }

                filesPaths.push_back(pathName.path().generic_string());
            }
        }

        return filesPaths;
    }

    std::string findPlugin(SearchPath const &searchPaths,
                           const std::string &pluginName) {
        for (const auto &searchPath : searchPaths) {
            if (!fs::exists(searchPath)) {
                continue;
            }

            for (const auto &pluginPathName : make_iterator_range(
                     directory_iterator(searchPath), directory_iterator())) {
                /// Must be a regular file
                /// @todo does this mean symlinks get excluded?
                if (!fs::is_regular_file(pluginPathName)) {
                    continue;
                }

                const auto pluginCandidate = fs::path(pluginPathName);

                /// Needs right extension
                if (pluginCandidate.extension().generic_string() !=
                    OSVR_PLUGIN_EXTENSION) {
                    continue;
                }
                const auto pluginBaseName =
                    pluginCandidate.filename().stem().generic_string();

#if defined(_MSC_VER) && !defined(NDEBUG)
                // Visual C++ debug runtime: we append to the plugin name.
                const std::string decoratedPluginName =
                    pluginName + OSVR_PLUGIN_DEBUG_SUFFIX;
#else
                const std::string &decoratedPluginName = pluginName;
#endif

                /// If the name is right or has the manual load suffix, this is
                /// a good one.
                if ((pluginBaseName == pluginName) ||
                    (pluginBaseName ==
                     decoratedPluginName + OSVR_PLUGIN_IGNORE_SUFFIX)) {
                    return pluginPathName.path().generic_string();
                }
            }
        }

        return std::string();
    }

} // namespace pluginhost
} // namespace osvr
