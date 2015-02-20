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

        SearchPath path;
#ifdef _MSC_VER
        path = (root / OSVR_PLUGIN_DIR / CMAKE_INTDIR).string();
#else
        path = (root / OSVR_PLUGIN_DIR).string());
#endif
        return path;
    }

    FileList getAllFilesWithExt(SearchPath dirPath, const std::string &ext){

        FileList filesPaths;
        boost::filesystem::path directoryPath(dirPath);

        //make sure that directory exists
        if (!boost::filesystem::exists(directoryPath)){
            return filesPaths;
        }

        boost::filesystem::recursive_directory_iterator iter(directoryPath);
        boost::filesystem::recursive_directory_iterator endIter;
        
        //get a list of files inside the dir that match the extension
        while (iter != endIter){

            if (boost::filesystem::is_regular_file(*iter) && iter->path().extension() == ext){
                
                //convert to forward slash
                std::string path = iter->path().string();
                std::replace(path.begin(), path.end(), '\\', '/');
                filesPaths.push_back(path);
            }
            iter++;
        }

        return filesPaths;
    }


} // namespace pluginhost
} // namespace osvr