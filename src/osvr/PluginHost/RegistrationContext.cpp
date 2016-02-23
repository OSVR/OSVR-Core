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
#include <osvr/PluginHost/SearchPath.h>
#include <osvr/PluginHost/PathConfig.h>
#include "PluginSpecificRegistrationContextImpl.h"
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <libfunctionality/LoadPlugin.h>
#include <boost/filesystem.hpp>
#include <boost/noncopyable.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/algorithm/string/predicate.hpp>

// Standard includes
#include <algorithm>
#include <iterator>

namespace osvr {
namespace pluginhost {

    namespace fs = boost::filesystem;

    struct RegistrationContext::Impl : private boost::noncopyable {
        /// constructor - creates and caches the plugin search path
        Impl() : pluginPaths(pluginhost::getPluginSearchPath()) {}

        const std::vector<std::string> pluginPaths;
    };
    RegistrationContext::RegistrationContext() : m_impl(new Impl) {}

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

    static inline bool tryLoadingPlugin(libfunc::PluginHandle &plugin,
                                        std::string const &name,
                                        OSVR_PluginRegContext ctx,
                                        bool shouldRethrow = false) {
        OSVR_DEV_VERBOSE("Trying to load a plugin with the name " << name);
        try {
            plugin = libfunc::loadPluginByName(name, ctx);
            return true;
        } catch (std::runtime_error const &e) {
            OSVR_DEV_VERBOSE("Failed: " << e.what());
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

        const std::string pluginPathName =
            pluginhost::findPlugin(m_impl->pluginPaths, pluginName);
        if (pluginPathName.empty()) {
            throw std::runtime_error("Could not find plugin named " +
                                     pluginName);
        }

        const auto pluginPathNameNoExt =
            (fs::path(pluginPathName).parent_path() /
             fs::path(pluginPathName).stem())
                .generic_string();
        PluginRegPtr pluginReg(
            PluginSpecificRegistrationContext::create(pluginName));
        pluginReg->setParent(*this);

        libfunc::PluginHandle plugin;
        auto ctx = pluginReg->extractOpaquePointer();

        bool success = tryLoadingPlugin(plugin, pluginPathName, ctx) ||
                       tryLoadingPlugin(plugin, pluginPathNameNoExt, ctx, true);
        if (!success) {
            throw std::runtime_error(
                "Unusual error occurred trying to load plugin named " +
                pluginName);
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
            OSVR_DEV_VERBOSE("Examining plugin '" << plugin << "'...");
            const auto pluginBaseName =
                fs::path(plugin).filename().stem().generic_string();
            if (boost::iends_with(pluginBaseName, OSVR_PLUGIN_IGNORE_SUFFIX)) {
                OSVR_DEV_VERBOSE(
                    "Ignoring manual-load plugin: " << pluginBaseName);
                continue;
            }

            try {
                loadPlugin(pluginBaseName);
                OSVR_DEV_VERBOSE(
                    "Successfully loaded plugin: " << pluginBaseName);
            } catch (const std::exception &e) {
                OSVR_DEV_VERBOSE("Failed to load plugin " << pluginBaseName
                                                          << ": " << e.what());
            } catch (...) {
                OSVR_DEV_VERBOSE("Failed to load plugin "
                                 << pluginBaseName << ": Unknown error.");
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
