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
#include <osvr/PluginHost/SearchPath.h>
#include "BinaryLocation.h"

// Library/third-party includes
#include <boost/filesystem.hpp>

// Standard includes
// - none

namespace osvr {
namespace pluginhost {
    SearchPath getPluginSearchPath() {
        boost::filesystem::path exeLocation(getBinaryLocation());

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
        auto root = binDir.parent_path();

        SearchPath paths;
#ifdef _MSC_VER
        paths.push_back((root / OSVR_PLUGIN_DIR / CMAKE_INTDIR).string());
#endif
        paths.push_back((root / OSVR_PLUGIN_DIR).string());

        // TODO add user's home directory to search path

        return paths;
    }

    FileList getAllFilesWithExt(SearchPath dirPath, const std::string &ext) {
        FileList filesPaths;

        for (const auto& path : dirPath) {
            boost::filesystem::path directoryPath(path);

            // Make sure that directory exists
            if (!boost::filesystem::exists(directoryPath)) {
                continue;
            }

            // Get a list of files inside the dir that match the extension
            for (const auto& pathName : recursive_directory_range(directoryPath)) {
                if (!boost::filesystem::is_regular_file(pathName) || pathName.path().extension() != ext)
                    continue;

                filesPaths.push_back(pathName.path().generic_string());
            }
        }

        return filesPaths;
    }

    std::string findPlugin(const std::string& pluginName) {
        auto searchPaths = getPluginSearchPath();
        for (const auto& searchPath : searchPaths) {
            if (!boost::filesystem::exists(searchPath))
                continue;

            for (const auto& pluginPathName : directory_range(searchPath)) {
                if (!boost::filesystem::is_regular_file(pluginPathName))
                    continue;

                const std::string pluginBaseName = boost::filesystem::path(pluginPathName).filename().stem().generic_string();
                if (pluginBaseName == pluginName)
                    return pluginPathName.path().generic_string();
            }
        }

        return std::string();
    }

} // namespace pluginhost
} // namespace osvr

