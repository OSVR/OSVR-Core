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

#ifdef _MSC_VER
        const SearchPath path = (root / OSVR_PLUGIN_DIR / CMAKE_INTDIR).string();
#else
        const SearchPath path = (root / OSVR_PLUGIN_DIR).string();
#endif
        return path;
    }

    FileList getAllFilesWithExt(SearchPath dirPath, const std::string &ext) {
        FileList filesPaths;
        boost::filesystem::path directoryPath(dirPath);

        // Make sure that directory exists
        if (!boost::filesystem::exists(directoryPath)) {
            return filesPaths;
        }

        // Get a list of files inside the dir that match the extension
        for (const auto& path : recursive_directory_range(directoryPath)) {
            if (!boost::filesystem::is_regular_file(path) || path.path().extension() != ext)
                continue;

            filesPaths.push_back(path.path().generic_string());
        }

        return filesPaths;
    }

} // namespace pluginhost
} // namespace osvr

