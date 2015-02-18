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
#include <osvr/PluginHost/PathConfig.h>

// Library/third-party includes
#include <boost/filesystem.hpp>

// Standard includes
// - none

/* PathConfig.h contains something like the following:

    #define OSVR_PLUGIN_DIR "bin"
    #define OSVR_PLUGIN_EXTENSION ".dll"

*/

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

        return paths;
    }

} // namespace pluginhost
} // namespace osvr