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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/JointClientKit/JointClientKitC.h>
#include "JointClientContext.h"
#include <osvr/Server/Server.h>
#include <osvr/Util/MacroToolsC.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
// - none

// Standard includes
#include <functional>
#include <vector>
#include <memory>
#include <string>

using osvr::server::Server;

using ServerOp = std::function<void(Server &)>;

struct OSVR_JointClientContextOptsObject {
    void apply(osvr::server::Server &s) {
        for (auto &op : operations) {
            op(s);
        }
    }

    template <typename F> void enqueue(F &&f) {
        operations.emplace_back(std::forward<F>(f));
    }
    std::vector<ServerOp> operations;

    bool haveAutoload = false;
};

OSVR_JointClientOpts osvrJointClientCreateOptions() {
    return new OSVR_JointClientContextOptsObject;
}

#define OSVR_CHECK_OPTS                                                        \
    OSVR_UTIL_MULTILINE_BEGIN                                                  \
    if (nullptr == opts) {                                                     \
        OSVR_DEV_VERBOSE(                                                      \
            "Can't modify a null JointClientKit options object!");             \
        return OSVR_RETURN_FAILURE;                                            \
    }                                                                          \
    OSVR_UTIL_MULTILINE_END

#define OSVR_OPTS_SINGLE_SHOT(MEMBER, DESC)                                    \
    OSVR_UTIL_MULTILINE_BEGIN                                                  \
    if (opts->MEMBER) {                                                        \
        OSVR_DEV_VERBOSE("Already queued " DESC                                \
                         " -- can't do it a second time!");                    \
        return OSVR_RETURN_FAILURE;                                            \
    }                                                                          \
    opts->MEMBER = true;                                                       \
    OSVR_UTIL_MULTILINE_END

#define OSVR_OPTS_CHECK_STRING(PARAM, DESC)                                    \
    OSVR_UTIL_MULTILINE_BEGIN                                                  \
    if (nullptr == (PARAM) || '\0' == (PARAM)[0]) {                            \
        OSVR_DEV_VERBOSE("Invalid string passed for  " DESC                    \
                         " -- can't be null or empty!");                       \
        return OSVR_RETURN_FAILURE;                                            \
    }                                                                          \
    OSVR_UTIL_MULTILINE_END

OSVR_ReturnCode
osvrJointClientOptionsAutoloadPlugins(OSVR_JointClientOpts opts) {
    OSVR_CHECK_OPTS;
    OSVR_OPTS_SINGLE_SHOT(haveAutoload, "autoload of plugins");
    opts->enqueue([](Server &s) { s.loadAutoPlugins(); });
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrJointClientOptionsLoadPlugin(OSVR_JointClientOpts opts,
                                                 const char *pluginName) {
    OSVR_CHECK_OPTS;
    OSVR_OPTS_CHECK_STRING(pluginName, "plugin name");
    auto name = std::string{pluginName};
    opts->enqueue([name](Server &s) { s.loadPlugin(name); });
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrJointClientOptionsInstantiateDriver(
    OSVR_JointClientOpts opts, const char *pluginName, const char *driverName,
    const char *params) {
    OSVR_CHECK_OPTS;
    OSVR_OPTS_CHECK_STRING(pluginName, "plugin name");
    OSVR_OPTS_CHECK_STRING(driverName, "driver name");
    auto plugin = std::string{pluginName};
    auto driver = std::string{driverName};
    auto p = std::string{(params ? params : "")};
    opts->enqueue([plugin, driver, p](Server &s) {
        s.instantiateDriver(plugin, driver, p);
    });
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrJointClientOptionsAddAlias(OSVR_JointClientOpts opts,
                                               const char *path,
                                               const char *source) {
    OSVR_CHECK_OPTS;
    OSVR_OPTS_CHECK_STRING(path, "path");
    OSVR_OPTS_CHECK_STRING(source, "source");
    auto p = std::string{path};
    auto src = std::string{source};
    opts->enqueue([p, src](Server &s) { s.addAlias(p, src); });
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrJointClientOptionsAddAliases(OSVR_JointClientOpts opts,
                                                 const char *aliases) {
    OSVR_CHECK_OPTS;
    OSVR_OPTS_CHECK_STRING(aliases, "aliases JSON");
    auto json = std::string{aliases};
    opts->enqueue([json](Server &s) { s.addAliases(json); });
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrJointClientOptionsAddString(OSVR_JointClientOpts opts,
                                                const char *path,
                                                const char *s) {
    OSVR_CHECK_OPTS;
    OSVR_OPTS_CHECK_STRING(path, "path");
    OSVR_OPTS_CHECK_STRING(s, "string value");
    auto p = std::string{path};
    auto str = std::string{s};
    opts->enqueue([p, str](Server &s) { s.addString(p, str); });
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode
osvrJointClientOptionsTriggerHardwareDetect(OSVR_JointClientOpts opts) {
    OSVR_CHECK_OPTS;
    opts->enqueue([](Server &s) { s.triggerHardwareDetect(); });
    return OSVR_RETURN_SUCCESS;
}

OSVR_ClientContext osvrJointClientInit(const char applicationIdentifier[],
                                       OSVR_JointClientOpts opts) {
    try {
        using OptionPtr = std::unique_ptr<OSVR_JointClientContextOptsObject>;
        // Take ownership of options, if any
        OptionPtr opt(opts);

        using JointContextPtr =
            std::unique_ptr<osvr::client::JointClientContext>;
        // Make the context.
        auto ctx = JointContextPtr{
            osvr::common::makeContext<osvr::client::JointClientContext>(
                applicationIdentifier)};

        if (opt) {
            opt->apply(ctx->getServer());
        } else {
            // Default behavior when no options are passed.
            ctx->getServer().loadAutoPlugins();
            ctx->getServer().triggerHardwareDetect();
        }
        // Transfer ownership to the client app.
        return ctx.release();
    } catch (std::exception const &e) {
        OSVR_DEV_VERBOSE(
            "Caught an exception in osvrJointClientInit: " << e.what());
    } catch (...) {

        OSVR_DEV_VERBOSE(
            "Caught an unrecognized exception type in osvrJointClientInit.");
    }

    return nullptr;
}
