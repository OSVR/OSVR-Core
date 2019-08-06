/** @file
    @brief Implementation

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>

*/

// Copyright 2014 Sensics, Inc.
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

#ifndef __ANDROID__
#define OSVR_DEV_VERBOSE_DISABLE
#endif

// Internal Includes
#include <osvr/PluginHost/RegistrationContext.h>

#include "PluginSpecificRegistrationContextImpl.h"
#include <osvr/PluginHost/PathConfig.h>
#include <osvr/PluginHost/SearchPath.h>
#include <osvr/Util/Log.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/noncopyable.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <libfunctionality/LoadPlugin.h>

// Standard includes
#include <algorithm>
#include <iterator>

namespace osvr {
namespace pluginhost {
    static const auto PLUGIN_HOST_LOGGER_NAME = "PluginHost";
    namespace fs = boost::filesystem;

    struct RegistrationContext::Impl : private boost::noncopyable {
        /// constructor - creates and caches the plugin search path
        Impl() : pluginPaths(pluginhost::getPluginSearchPath()) {}

        const std::vector<std::string> pluginPaths;
    };

    RegistrationContext::RegistrationContext()
        : m_impl(new Impl),
          m_logger(util::log::make_logger(PLUGIN_HOST_LOGGER_NAME)) {}

    RegistrationContext::~RegistrationContext() {
        // Reset the plugins in reverse order.
        for (auto &ptr : m_regMap | boost::adaptors::map_values |
                             boost::adaptors::reversed) {
            ptr.reset();
        }
    }

    template <typename MapType>
    static inline bool isPluginLoaded(MapType const &regMap,
                                      std::string const &pluginName) {
        return (regMap.find(pluginName) != end(regMap));
    }

    /// Only called by manually-named plugins to load: the plugins that are
    /// loaded automatically don't go through this function.
    static inline bool tryLoadingPlugin(util::log::Logger &log,
                                        libfunc::PluginHandle &plugin,
                                        std::string const &name,
                                        OSVR_PluginRegContext ctx,
                                        bool shouldRethrow = false) {

        log.debug() << "Trying to load a plugin with the name "
                    << name;
        try {
            plugin = libfunc::loadPluginByName(name, ctx);
            return true;
        } catch (std::runtime_error const &e) {
            log.debug() << "Failed: " << e.what();
            if (shouldRethrow) {
                throw;
            }
            return false;
        } catch (...) {
            throw;
        }
    }

    void RegistrationContext::loadPlugin(std::string const &pluginName) {
        if (isPluginLoaded(m_regMap, pluginName)) {
            throw std::runtime_error("Already loaded a plugin named " +
                                     pluginName);
        }

        PluginRegPtr pluginReg(
            PluginSpecificRegistrationContext::create(pluginName));
        pluginReg->setParent(*this);

        bool success = false;
        libfunc::PluginHandle plugin;
        auto ctx = pluginReg->extractOpaquePointer();
        const std::string pluginPathName =
            pluginhost::findPlugin(m_impl->pluginPaths, pluginName);
        if (pluginPathName.empty()) {
            // was the plugin pre-loaded or statically linked? Try loading
            // it by name.
            success = tryLoadingPlugin(*m_logger, plugin, pluginName, ctx);
            if (!success) {
                throw std::runtime_error("Could not find plugin named " +
                                         pluginName);
            }
        }

        if (!success) {
            const auto pluginPathNameNoExt =
                (fs::path(pluginPathName).parent_path() /
                 fs::path(pluginPathName).stem())
                    .generic_string();

            success =
                tryLoadingPlugin(*m_logger, plugin, pluginPathName, ctx) ||
                tryLoadingPlugin(*m_logger, plugin, pluginPathNameNoExt, ctx,
                                 true);
            if (!success) {
                throw std::runtime_error(
                    "Unusual error occurred trying to load plugin named " +
                    pluginName);
            }
        }
        pluginReg->takePluginHandle(plugin);
        adoptPluginRegistrationContext(pluginReg);
    }

    void RegistrationContext::loadPlugins() {
        // Build a list of all the plugins we can find
        auto pluginPathNames = pluginhost::getAllFilesWithExt(
            m_impl->pluginPaths, OSVR_PLUGIN_EXTENSION);

        // Load all of the non-.manualload plugins
        for (const auto &plugin : pluginPathNames) {
            m_logger->debug() << "Examining plugin '" << plugin << "'...";
            const auto pluginBaseName =
                fs::path(plugin).filename().stem().generic_string();
            if (boost::iends_with(pluginBaseName, OSVR_PLUGIN_IGNORE_SUFFIX)) {
                m_logger->debug() << "Ignoring manual-load plugin: "
                                  << pluginBaseName;
                continue;
            }

#if defined(_MSC_VER)
            // Visual C++ debug runtime: we append to the plugin name. Must only
            // load debug plugins iff we're a debug server
            const auto isDebugRuntimePlugin =
                boost::iends_with(pluginBaseName, OSVR_PLUGIN_DEBUG_SUFFIX);
#if defined(NDEBUG)
            /// This is a non-debug build.
            if (isDebugRuntimePlugin) {
                m_logger->debug() << "Ignoring debug-runtime plugin: "
                                  << pluginBaseName;
                continue;
            }
#else
            /// This is a debug build
            if (!isDebugRuntimePlugin) {
                m_logger->debug() << "Ignoring non-debug-runtime plugin: "
                                  << pluginBaseName;
                continue;
            }
#endif // NDEBUG
#endif // _MSC_VER

            try {
                loadPlugin(pluginBaseName);
                m_logger->debug() << "Successfully loaded plugin: "
                                  << pluginBaseName;
            } catch (const std::exception &e) {
                m_logger->warn() << "Failed to load plugin " << pluginBaseName
                                 << ": " << e.what();
            } catch (...) {
                m_logger->warn() << "Failed to load plugin " << pluginBaseName
                                 << ": Unknown error.";
            }
        }
    }

    void RegistrationContext::adoptPluginRegistrationContext(PluginRegPtr ctx) {
        /// This set parent might be a duplicate, but won't be if the plugin reg
        /// ctx is not created by loadPlugin above.
        ctx->setParent(*this);

        m_regMap.insert(std::make_pair(ctx->getName(), ctx));
    }

    void RegistrationContext::triggerHardwareDetect() {
        for (auto &pluginPtr : m_regMap | boost::adaptors::map_values) {
            pluginPtr->triggerHardwareDetectCallbacks();
        }
    }

    void
    RegistrationContext::instantiateDriver(const std::string &pluginName,
                                           const std::string &driverName,
                                           const std::string &params) const {
        auto pluginIt = m_regMap.find(pluginName);
        if (pluginIt == end(m_regMap)) {
            throw std::runtime_error("Could not find plugin named " +
                                     pluginName);
        }
        pluginIt->second->instantiateDriver(driverName, params);
    }

    util::AnyMap &RegistrationContext::data() { return m_data; }

    util::AnyMap const &RegistrationContext::data() const { return m_data; }
} // namespace pluginhost
} // namespace osvr
