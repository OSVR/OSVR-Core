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

// Internal Includes
#include <osvr/Util/Directories.h>
#include <osvr/Util/GetEnvironmentVariable.h>
#include <osvr/Util/PlatformConfig.h>

// Library/third-party includes
#include <boost/filesystem.hpp>

// Standard includes
#include <string>
#include <vector>

// Platform-specific includes
#if defined(OSVR_WINDOWS)
#include <Shlobj.h>     // for SHGetKnownFolderPath()
#include <Knownfolders.h>
#include <codecvt>
#include <locale>
#elif defined(OSVR_LINUX)
#include <boost/algorithm.string.hpp>   // for boost::split()
#endif

namespace osvr {
namespace util {

// https://github.com/ActiveState/appdirs/blob/master/appdirs.py
// https://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html
// http://www.filewatcher.com/m/BPFileSystem.pdf.457603-0.html
// https://support.microsoft.com/en-us/kb/310294#XSLTH3194121123120121120120
//

namespace {

#if defined(OSVR_WINDOWS)
inline std::string to_string(const std::wstring& s)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.to_bytes(s);
}

// https://msdn.microsoft.com/en-us/library/dd378457.aspx
inline std::string getKnownFolderPath(KNOWNFOLDERID known_folder_id)
{
    LPWSTR wide_path = nullptr;
    const auto result = SHGetKnownFolderPath(&known_folder_id, 0, nullptr, wide_path);
    if (SUCCEEDED(result)) {
        return to_string(wide_path);
    }
    return "";
}
#endif // OSVR_WINDOWS

#if defined(OSVR_LINUX)
// The following functions return paths specified by the XDG Base Directory
// Specification
// <https://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html>.
// These functions apply to modern Linux installations.

enum class XDGDirectory {
    XDG_CACHE_HOME,  ///< user-specific non-essential data files
    XDG_CONFIG_DIRS, ///< search for configuration files in addition to the XDG_CONFIG_HOME
    XDG_CONFIG_HOME, ///< user-specific configuration files
    XDG_DATA_DIRS,   ///< search for data files in addition to the XDG_DATA_HOME
    XDG_DATA_HOME,   ///< user-specific data files
    XDG_RUNTIME_DIR  ///< user-specific non-essential runtime files (sockets, named pipes, etc.)
};

inline std::vector<std::string> getXDGDirectoriesEnvOrDefault(const std::string& env, const std::vector<std::string>& default)
{
    const auto dirs_str = getEnvironmentVariable(env);
    if (!dirs_str || dirs_str->empty())
        return default;

    std::vector<std::string> dirs;
    boost::split(dirs, *env, boost::is_any_of(":"));
    return dirs;
}

inline std::vector<std::string> getXDGDirectories(const XDGDirectory dir)
{
    if (XDG_CONFIG_DIRS == dir) {
        return getXDGDirectoriesEnvOrDefault("XDG_CONFIG_DIRS", {"/usr/local/share", "/usr/share"});
    } else if (XDG_DATA_DIRS == dir) {
        return getXDGDirectoriesEnvOrDefault("XDG_DATA_DIRS", {"/etc/xdg"});
    }

    return {};
}

inline std::string getXDGDirectoryEnvOrDefault(const std::string& env, const std::string& default)
{
    const auto dir = getEnvironmentVariable(env);
    if (!dir || dir->empty())
        return default;
    return *dir;
}

inline std::string getXDGDirectory(const XDGDirectory dir)
{
    const auto home_dir = *getEnvironmentVariable("HOME");

    if (XDG_CACHE_HOME == dir) {
        return getXDGDirectoryEnvOrDefault("XDG_CACHE_HOME", home_dir + "/.cache");
    } else if (XDG_CONFIG_HOME == dir) {
        return getXDGDirectoryEnvOrDefault("XDG_CONFIG_HOME", home_dir + "/.config");
    } else if (XDG_DATA_HOME == dir) {
        return getXDGDirectoryEnvOrDefault("XDG_DATA_HOME", home_dir + "/.local/share");
    } else if (XDG_RUNTIME_DIR == dir) {
        // FIXME (use /run/user/~ or /tmp or /var/tmp etc.)
        return getXDGDirectoryEnvOrDefault("XDG_RUNTIME_DIR", "/tmo");
    } else {
        // TODO
        return "";
    }
}

// https://freedesktop.org/wiki/Software/xdg-user-dirs/
inline std::string getXDGUserDirectory(const std::string& type, const std::string& fallback)
{
    FILE *file;
    char *home_dir, *config_home, *config_file;
    char buffer[512];
    char *user_dir;
    char *p, *d;
    int len;
    int relative;

    const auto home_dir = getEnvironmentVariable("HOME");

    if (!home_dir) {
        return fallback;
    }

    const auto config_home = getXDGDirectory(XDG_CONFIG_HOME);
    const auto config_file = config_home + "/user-dirs.dirs";

    std::ifstream input(config_file);
    for (std::string line; std::getline(input, line); ) {

        if (line.find("XDG_" + type + "_DIR") != std::end(line)) {

        }

        if (relative)
        {
            user_dir = (char*) malloc (strlen (home_dir) + 1 + strlen (p) + 1);
            if (user_dir == NULL)
                goto error2;

            strcpy (user_dir, home_dir);
            strcat (user_dir, "/");
        }
        else
        {
            user_dir = (char*) malloc (strlen (p) + 1);
            if (user_dir == NULL)
                goto error2;

            *user_dir = 0;
        }

        d = user_dir + strlen (user_dir);
        while (*p && *p != '"')
        {
            if ((*p == '\\') && (*(p+1) != 0))
                p++;
            *d++ = *p++;
        }
        *d = 0;
    }
error2:
    fclose (file);

    if (user_dir)
        return user_dir;

error:
    if (fallback)
        return strdup (fallback);
    return NULL;
}

/**
 * xdg_user_dir_lookup:
 * @type: a string specifying the type of directory
 * @returns: a newly allocated absolute pathname
 *
 * Looks up a XDG user directory of the specified type.
 * Example of types are "DESKTOP" and "DOWNLOAD".
 *
 * The return value is always != NULL (unless out of memory),
 * and if a directory
 * for the type is not specified by the user the default
 * is the home directory. Except for DESKTOP which defaults
 * to ~/Desktop.
 *
 * The return value is newly allocated and must be freed with
 * free().
 **/
static char *
xdg_user_dir_lookup (const char *type)
{
  char *dir, *home_dir, *user_dir;

  dir = xdg_user_dir_lookup_with_fallback (type, NULL);
  if (dir != NULL)
    return dir;

  home_dir = getenv ("HOME");

  if (home_dir == NULL)
    return strdup ("/tmp");

  /* Special case desktop for historical compatibility */
  if (strcmp (type, "DESKTOP") == 0)
    {
      user_dir = (char*) malloc (strlen (home_dir) + strlen ("/Desktop") + 1);
      if (user_dir == NULL)
        return NULL;

      strcpy (user_dir, home_dir);
      strcat (user_dir, "/Desktop");
      return user_dir;
    }

  return strdup (home_dir);
}
#endif // OSVR_LINUX

} // end anonymous namespace

std::vector<std::string> getDirectories(DirectoryType dirtype, const std::string& appname, const std::string& author, const std::string& version)
{
    std::vector<std::string> dirs;
    // TODO
    return dirs;
}

std::string getUserDataDir(const std::string& appname, const std::string& author, const std::string& version)
{
    // Typical user data directories are:
    //     Mac OS X: ~/Library/Application Support/<appname>/<version>
    //     Unix:     $XDG_DATA_HOME/<appname>/<version>
    //     Windows:  C:\Users\<username>\AppData\Local\<author>\<appname>\<version>

    namespace fs = boost::filesystem;
    fs::path path;

#if defined(OSVR_MACOSX)
    const auto home_dir = getEnvironmentVariable("HOME");
    if (home_dir) {
        path = *home_dir;
    }
    path /= fs::path("Library") / "Application Support";
    if (!appname.empty()) {
        path /= appname;
    }
#elif defined(OSVR_WINDOWS)
    path = getKnownFolderPath(FOLDERID_LocalAppData);
    if (!author.empty()) {
        if (author != appname) {
            path /= author / appname;
        } else {
            path /= appname;
        }
    }
#elif defined(OSVR_LINUX)
    path = getXDGDirectory(XDG_DATA_HOME);
    if (!appname.empty())
        path /= appname;
#else
#warning "osvr::util::getUserDataDir() is not yet supported on this platform."
#endif // platform-specific code

    if (!appname.empty() && !version.empty()) {
        path /= version;
    }

    return path.string();
}

std::string getUserConfigDir(const std::string& appname, const std::string& author, const std::string& version)
{
    // Typical user config directories are:
    //   Mac OS X: same as user_data_dir
    //   Windowns: same as user_data_dir
    //   Unix:     XDG_CONFIG_HOME

    namespace fs = boost::filesystem;
    fs::path path;

#if defined(OSVR_WINDOWS) || defined(OSVR_MACOSX)
    path = getUserDataDir(appname, author, version);
#elif defined(OSVR_LINUX)
    path = getXDGDirectory(XDG_CONFIG_HOME);

    if (!appname.empty())
        path /= appname;
#else
#warning "osvr::util::getUserConfigDir() is not yet supported on this platform."
#endif

    if (!appname.empty() && !version.empty())
        path /= version;

    return path.string();
}

std::string getUserCacheDir(const std::string& appname, const std::string& author, const std::string& version)
{
    // Typical user cache directories are:
    //   Mac OS X: ~/Library/Caches/<appname>/<version>
    //             ~/Library/Caches/<appname>
    //             ~/Library/Caches
    //   Unix:     $XDG_CACHE_HOME/<appname>/<version>
    //   Unix:     $XDG_CACHE_HOME/<appname>
    //             $XDG_CACHE_HOME
    //   Windows:  %LocalAppData%\<author>\<appname>\<version>\Cache
    //             %LocalAppData%\<author>\<appname>\Cache
    //             %LocalAppData%\<appname>\Cache
    //             %LocalAppData%\Cache

    namespace fs = boost::filesystem;
    fs::path path;

#if defined(OSVR_MACOSX)
    path = *getEnvironmentVariable("HOME") / fs::path("Library") / "Caches";
    if (!appname.empty())
        path /= appname;
    if (!appname.empty() && !version.empty())
        path /= version;
#elif defined(OSVR_WINDOWS)
    path = getKnownFolderPath(FOLDERID_LocalAppData);

    if (!author.empty())
        path /= author;

    if (!appname.empty())
        path /= appname;

    if (!version.empty())
        path /= version;

    path /= "Cache";

#elif defined(OSVR_LINUX)
    path = getXDGDirectory(XDG_CACHE_HOME);
    if (!appname.empty())
        path /= appname;
    if (!appname.empty() && !version.empty())
        path /= version;
#else
#warning "osvr::util::getUserCacheDir() is not yet supported on this platform."
#endif

    return path.string();
}

std::string getUserStateDir(const std::string& appname, const std::string& author, const std::string& version)
{
    // Typical user state directories are:
    //   Mac OS X:  same as user_data_dir
    //   Windows:   same as user_data_dir
    //   Unix:      $XDG_STATE_HOME/<appname>

    namespace fs = boost::filesystem;
    fs::path path;

#if defined(OSVR_MACOSX) || defined(OSVR_WINDOWS)
    return getUserDataDir(appname, author, version);
#elif defined(OSVR_LINUX)
    path = getXDGDirectory(XDG_STATE_HOME);
    if (!appname.empty()) {
        path /= appname;
        if (!version.empty()) {
            path /= version;
        }
    }

#else
#warning "osvr::util::getUserStateDir() is not yet supported on this platform."
#endif

    return path.string();
}

std::string getUserLogDir(const std::string& appname, const std::string& author, const std::string& version)
{
    // Typical user state directories are:
    //   Mac OS X:  ~/Library/Logs/<appname>
    //   Windows:   %LocalAppData%/<author>/<appname>/Logs
    //   Unix:      $XDG_CACHE_HOME/<appname>/logs

    namespace fs = boost::filesystem;
    fs::path log_dir;

#if defined(OSVR_LINUX)
    // There's currently no great location for storing log files in the
    // XDG system. (See the STATE proposal by Debian
    // <https://wiki.debian.org/XDGBaseDirectorySpecification#Proposal:_STATE_directory>.)
    // So for now, we'll store our log files in the $XDG_CACHE_HOME
    // directory.
    //
    // $XDG_CACHE_HOME defines the base directory relative to which user
    // specific non-essential data files should be stored. If
    // $XDG_CACHE_HOME is either not set or empty, a default equal to
    // $HOME/.cache should be used.
    log_dir /= getXDGDirectory(XDG_CACHE_HOME);

    if (!appname.empty())
        log_dir /= appname;

    log_dir /= "logs";
#elif defined(OSVR_MACOSX)
    auto home_dir = getEnvironmentVariable("HOME");
    if (home_dir)
        log_dir = *home_dir;

    log_dir /= "Library" / fs::path("Logs");

    if (!appname.empty())
        log_dir /= appname;
#elif defined(OSVR_WINDOWS)
    log_dir = getKnownFolderPath(FOLDERID_LocalAppData);
    if (!author.empty())
        log_dir /= author;

    if (!appname.empty())
        log_dir /= appname;

    log_dir /= "Logs";
#else
#warning "osvr::util::getUserLogDir() is not yet supported on this platform."
#endif

    return log_dir.string();
}

std::string getSiteDataDir(const std::string& appname, const std::string& author, const std::string& version)
{
    // Typical site data directories are:
    //   Mac OS X: /Library/Application Support/<appname>/<version>
    //             /Library/Application Support/<appname>
    //   Linux:    $HDG_DATA_DIRS[0]/<appname>/<version>
    //             $HDG_DATA_DIRS[0]/<appname>
    //   Windows:  C:\ProgramData\<author>\<appname>\<version>
    //             C:\ProgramData\<author>\<appname>
    //             C:\ProgramData\<appname>

    namespace fs = boost::filesystem;
    fs::path path;

#if defined(OSVR_WINDOWS)
    path = getKnownFolderPath(FOLDERID_ProgramData);

    if (!appname.empty()) {
        if (!author.empty())
            path /= author;

        path /= appname;

        if (!version.empty())
            path /= version;
    }
#elif defined(OSVR_MACOSX)
    path = *getEnvironmentVariable("HOME");
    path /= "Library";
    path /= "Application Support";

    if (!appname.empty()) {
        path /= appname;

        if (!version.empty())
            path /= version;
    }
#elif defined(OSVR_LINUX)
    path = getXDGDirectories(XDG_DATA_DIRS)[0];

    if (!appname.empty()) {
        path /= appname;

        if (!version.empty())
            path /= version;
    }
#else
#warning "osvr::util::getSiteDataDir() is not yet supported on this platform."
#endif

    return path.string();
}

std::string getSiteConfigDir(const std::string& appname, const std::string& author, const std::string& version)
{
    // Typical site data directories are:
    //   Mac OS X: same as getSiteDataDir()
    //   Linux:    $HDG_CONFIG_DIRS[0]/<appname>/<version>
    //             $HDG_CONFIG_DIRS[0]/<appname>
    //   Windows:  same as getSiteDataDir()

    namespace fs = boost::filesystem;
    fs::path path;

#if defined(OSVR_WINDOWS) || defined(OSVR_MACOSX)
    return getSiteDataDir(appname, author, version);
#elif defined(OSVR_LINUX)
    path = getXDGDirectories(XDG_CONFIG_DIRS)[0];

    if (!appname.empty()) {
        path /= appname;

        if (!version.empty())
            path /= version;
    }
#else
#warning "osvr::util::getSiteConfigDir() is not yet supported on this platform."
#endif

    return path.string();
}

std::string getUserDesktopDir(const std::string& appname, const std::string& author, const std::string& version)
{
    // Typical site data directories are:
    //   Mac OS X: same as getSiteDataDir()
    //   Linux:    $HDG_CONFIG_DIRS[0]/<appname>/<version>
    //             $HDG_CONFIG_DIRS[0]/<appname>
    //   Windows:  same as getSiteDataDir()

    namespace fs = boost::filesystem;
    fs::path path;

#if defined(OSVR_WINDOWS)
    return getKnownFolderPath(FOLDERID_Desktop);
#elif defined(OSVR_MACOSX)
    // FIXME
#elif defined(OSVR_LINUX)
    // FIXME
#else
#warning "osvr::util::getSiteConfigDir() is not yet supported on this platform."
#endif

    return path.string();
}

/**
 * @brief Return full path to the requested directory type.
 *
 * @param dirtype The requested directory to return.
 */
std::string getDirectory(const DirectoryType dirtype, const std::string& appname, const std::string& author, const std::string& version)
{
    switch (dirtype) {
    case USER_DIRECTORY_DATA:
        return getUserDataDir(appname, author, version);
    case USER_DIRECTORY_CONFIG:
        return getUserConfigDir(appname, author, version);
    case USER_DIRECTORY_CACHE:
        return getUserCacheDir(appname, author, version);
    case USER_DIRECTORY_STATE:
        return getUserStateDir(appname, author, version);
    case USER_DIRECTORY_LOG:
        return getUserLogDir(appname, author, version);
    case SITE_DIRECTORY_DATA:
        return getSiteDataDir(appname, author, version);
    case SITE_DIRECTORY_CONFIG:
        return getSiteConfigDir(appname, author, version);
    case USER_DIRECTORY_DESKTOP:
        return getUserDesktopDir(appname, author, version);
#if 0
    case USER_DIRECTORY_DOCUMENTS:
        return getUserDocumentsDir(appname, author, version);
    case USER_DIRECTORY_DOWNLOAD:
        return getUserDownloadsDir(appname, author, version);
    case USER_DIRECTORY_MUSIC:
        return getUserMusicDir(appname, author, version);
    case USER_DIRECTORY_PICTURES:
        return getUserPicturesDir(appname, author, version);
    case USER_DIRECTORY_PUBLIC_SHARE:
        return getUserPublicShareDir(appname, author, version);
    case USER_DIRECTORY_TEMPLATES:
        return getUserTemplatesDir(appname, author, version);
    case USER_DIRECTORY_VIDEOS:
        return getUserVideosDir(appname, author, version);
    default:
#endif // not yet implemented
    }

    return "";
}

} // end namespace util
} // end namespace osvr

