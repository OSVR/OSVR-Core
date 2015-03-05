/** @file
    @brief Header

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

#ifndef INCLUDED_SearchPath_h_GUID_D9D19BF6_FEB5_4B82_17A4_C8C397C88523
#define INCLUDED_SearchPath_h_GUID_D9D19BF6_FEB5_4B82_17A4_C8C397C88523

// Internal Includes
#include <osvr/PluginHost/Export.h>
#include <osvr/PluginHost/PathConfig.h>

// Library/third-party includes
#include <boost/filesystem.hpp>

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
        getAllFilesWithExt(SearchPath dirPath, const std::string &ext);

    /// Given the name of a plugin, find the full path to the plugin library.
    OSVR_PLUGINHOST_EXPORT std::string findPlugin(const std::string& pluginName);

    /// Helper class that generates an iterable range of directory entries.
    class recursive_directory_range {
      public:
        typedef boost::filesystem::recursive_directory_iterator iterator;

        recursive_directory_range(boost::filesystem::path p) : m_path(p) {}

        iterator begin() {
            return boost::filesystem::recursive_directory_iterator(m_path);
        }

        iterator end() {
            return boost::filesystem::recursive_directory_iterator();
        }

      private:
        boost::filesystem::path m_path;
    };

    /// Helper class that generates an iterable range of directory entries.
    class directory_range {
      public:
        typedef boost::filesystem::directory_iterator iterator;

        directory_range(boost::filesystem::path p) : m_path(p) {}

        iterator begin() {
            return boost::filesystem::directory_iterator(m_path);
        }

        iterator end() {
            return boost::filesystem::directory_iterator();
        }

      private:
        boost::filesystem::path m_path;
    };

} // namespace pluginhost
} // namespace osvr

#endif // INCLUDED_SearchPath_h_GUID_D9D19BF6_FEB5_4B82_17A4_C8C397C88523
