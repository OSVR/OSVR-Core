/** @file
    @brief Implementation

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>

*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#define OSVR_DEV_VERBOSE_DISABLE

// Internal Includes
#include <osvr/PluginHost/RegistrationContext.h>
#include <osvr/PluginHost/SearchPath.h>
#include "PluginSpecificRegistrationContextImpl.h"
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <libfunctionality/LoadPlugin.h>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/algorithm/string/predicate.hpp>

// Standard includes
#include <algorithm>
#include <iterator>

namespace osvr {
namespace pluginhost {
    RegistrationContext::RegistrationContext() {}

    RegistrationContext::~RegistrationContext() {
        // Reset the plugins in reverse order.
        for (auto &ptr : m_regMap | boost::adaptors::map_values |
                             boost::adaptors::reversed) {
            ptr.reset();
        }
    }

    static inline bool tryLoadingPlugin(libfunc::PluginHandle & plugin, std::string const& name, OSVR_PluginRegContext ctx, bool shouldRethrow = false) {
        OSVR_DEV_VERBOSE("Trying to load a plugin with the name " << name);
        try {
            plugin = libfunc::loadPluginByName(
                name, ctx);
            return true;
        } catch (std::runtime_error const&e) {
            OSVR_DEV_VERBOSE("Failed: " << e.what());
            if (shouldRethrow) {
                throw;
            }
            return false;
        }
        catch (...) {
            throw;
        }
    }

    void RegistrationContext::loadPlugin(std::string const &pluginName) {
        const std::string pluginPathName = pluginhost::findPlugin(pluginName);
        if (pluginPathName.empty()) {
            throw std::runtime_error("Could not find plugin named " +
                                     pluginName);
        }

        const std::string pluginPathNameNoExt = (boost::filesystem::path(pluginPathName).parent_path() / boost::filesystem::path(pluginPathName).stem()).generic_string();
        PluginRegPtr pluginReg(
            PluginSpecificRegistrationContext::create(pluginName));
        pluginReg->setParent(*this);

        libfunc::PluginHandle plugin;
        auto ctx = pluginReg->extractOpaquePointer();

        bool success = tryLoadingPlugin(plugin, pluginPathName, ctx) || tryLoadingPlugin(plugin, pluginPathNameNoExt, ctx, true);
        if (!success) {
            throw std::runtime_error("Unusual error occurred trying to load plugin named " +
                pluginName);
        }

        pluginReg->takePluginHandle(plugin);
        adoptPluginRegistrationContext(pluginReg);
    }

    void RegistrationContext::loadPlugins() {
        // Build a list of all the plugins we can find
        auto pluginPaths = pluginhost::getPluginSearchPath();
        for (const auto &pluginPath : pluginPaths) {
            OSVR_DEV_VERBOSE("Searching for plugins in " << pluginPath << "...");
        }
        auto pluginPathNames = pluginhost::getAllFilesWithExt(pluginPaths, OSVR_PLUGIN_EXTENSION);

        // Load all of the non-.manualload plugins
        for (const auto &plugin : pluginPathNames) {
            OSVR_DEV_VERBOSE("Examining plugin '" << plugin << "'...");
            const std::string pluginBaseName = boost::filesystem::path(plugin).filename().stem().generic_string();
            if (boost::iends_with(pluginBaseName, OSVR_PLUGIN_IGNORE_SUFFIX)) {
                OSVR_DEV_VERBOSE("Ignoring manual-load plugin: " << pluginBaseName);
                continue;
            }

            try {
                loadPlugin(pluginBaseName);
                OSVR_DEV_VERBOSE("Successfully loaded plugin: " << pluginBaseName);
            } catch (const std::exception &e) {
                OSVR_DEV_VERBOSE("Failed to load plugin " << pluginBaseName << ": " << e.what());
            } catch (...) {
                OSVR_DEV_VERBOSE("Failed to load plugin " << pluginBaseName << ": Unknown error.");
            }
        }
    }

    void RegistrationContext::adoptPluginRegistrationContext(PluginRegPtr ctx) {
        /// This set parent might be a duplicate, but won't be if the plugin reg
        /// ctx is not created by loadPlugin above.
        ctx->setParent(*this);

        m_regMap.insert(std::make_pair(ctx->getName(), ctx));
        OSVR_DEV_VERBOSE("RegistrationContext:\t"
                         "Adopted registration context for "
                         << ctx->getName());
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
