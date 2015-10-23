/** @file
    @brief Header

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

#ifndef INCLUDED_ConfigureServer_h_GUID_9D9EE94F_8F85_42BE_AFDD_4AA7B0EFA566
#define INCLUDED_ConfigureServer_h_GUID_9D9EE94F_8F85_42BE_AFDD_4AA7B0EFA566

// Internal Includes
#include <osvr/Server/Export.h>
#include <osvr/Server/ServerPtr.h>
#include <osvr/Util/UniquePtr.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>
#include <iosfwd>
#include <vector>

namespace osvr {
namespace server {
    /// @brief Internal implementation class
    class ConfigureServerData;

    /// @brief A class used for step-by-step construction and configuration of a
    /// server.
    ///
    /// For full configuration, run each named method, one by one, in the order
    /// they're seen in the header. (You only need to run one overload for each
    /// name)
    class ConfigureServer : boost::noncopyable {
      public:
        /// @brief Constructor
        OSVR_SERVER_EXPORT ConfigureServer();

        /// @brief Destructor
        OSVR_SERVER_EXPORT ~ConfigureServer();

        /// @brief Loads and parses the provided json
        /// @throws std::runtime_error if parsing errors occur.
        /// @brief json A string containing json configuration
        OSVR_SERVER_EXPORT void loadConfig(std::string const &json);
        /// @overload
        /// @brief json A stream containing json configuration
        OSVR_SERVER_EXPORT void loadConfig(std::istream &json);

        /// @brief Creates a server, choosing the factory method according to
        /// the passed JSON configuration
        ///
        /// It first looks for an object under the `server` key - if not
        /// present, it will assume defaults for below. This object is the
        /// context for the below elements.
        ///
        /// If `interface` is defined, it will open a server on that interface
        /// (with an empty string meaning all interfaces)
        ///
        /// Otherwise, `local` is assumed to be true. If it is false, it will
        /// open on all interfaces.
        ///
        /// `port` defaults to the assigned VRPN port (3883)
        ///
        /// @throws std::out_of_range if an invalid port (<1) is specified.
        OSVR_SERVER_EXPORT ServerPtr constructServer();

        /// @brief Container for plugin/driver names
        typedef std::vector<std::string> SuccessList;

        /// @brief Error information with attribution: `.first` field is
        /// plugin/driver name, `.second` field is exception text
        typedef std::pair<std::string, std::string> ErrorPair;

        /// @brief Container for plugin/driver names and error messages
        typedef std::vector<ErrorPair> ErrorList;

        /// @brief Loads the plugins contained in an array with key `plugins` in
        /// the configuration.
        ///
        /// Detailed results of the loading can be retrieved with
        /// getSuccessfulPlugins() and getFailedPlugins()
        ///
        /// @returns true if and only if all specified plugins loaded
        /// successfully.
        OSVR_SERVER_EXPORT bool loadPlugins();

        /// @name Results of loadPlugins()
        /// @{
        /// @brief Get a reference to the list of plugins successfully loaded by
        /// loadPlugins()
        OSVR_SERVER_EXPORT SuccessList const &getSuccessfulPlugins() const;

        /// @brief Get a reference to the list of plugins loadPlugins() tried
        /// but failed to load, along with any exception text.
        OSVR_SERVER_EXPORT ErrorList const &getFailedPlugins() const;
        /// @}

        /// @brief Configures and instantiates the drivers as specified.
        ///
        /// Looks for an array with the key of `drivers`, containing an array of
        /// objects. Each object is an instantiation, specifying `driver`,
        /// `plugin`, and `params` to pass along. `params` is typically nested
        /// JSON data.
        ///
        /// Detailed results of the loading can be retrieved with
        /// getSuccessfulInstantiations() and getSuccessfulInstantiations()
        ///
        /// @returns true if and only if all specified driver instances
        /// completed successfully.
        OSVR_SERVER_EXPORT bool instantiateDrivers();

        /// @name Results of instantiateDrivers()
        /// @{
        /// @brief Get a reference to the list of drivers successfully
        /// instantiated by instantiateDrivers()
        OSVR_SERVER_EXPORT SuccessList const &
        getSuccessfulInstantiations() const;

        /// @brief Get a reference to the list of drivers instantiateDrivers()
        /// tried to instantiate but failed, along with any exception text.
        OSVR_SERVER_EXPORT ErrorList const &getFailedInstantiations() const;
        /// @}

        OSVR_SERVER_EXPORT bool processRoutes();
        OSVR_SERVER_EXPORT bool processAliases();

        /// @brief Process any external devices in the config.
        /// @return true if any were found and loaded.
        OSVR_SERVER_EXPORT bool processExternalDevices();

        /// @brief Process a display element in the config.
        /// @return true if one was found and it was successfully loaded.
        OSVR_SERVER_EXPORT bool processDisplay();

        /// @brief Process a RenderManager config element in the config.
        /// @return true if one was found and it was successfully loaded.
        OSVR_SERVER_EXPORT bool processRenderManagerParameters();

        /// @brief Loads all plugins not marked for manual load.
        OSVR_SERVER_EXPORT void loadAutoPlugins();

      private:
        /// @brief Private implementation data structure.
        unique_ptr<ConfigureServerData> m_data;

        /// @brief Owning pointer for the server under
        /// construction/configuration.
        ServerPtr m_server;

        /// @name Results data of loadPlugins()
        /// @{
        SuccessList m_successfulPlugins;
        ErrorList m_failedPlugins;
        /// @}

        /// @name Results data of instantiateDrivers()
        /// @{
        SuccessList m_successfulInstances;
        ErrorList m_failedInstances;
        /// @}
    };
} // namespace server
} // namespace osvr

#endif // INCLUDED_ConfigureServer_h_GUID_9D9EE94F_8F85_42BE_AFDD_4AA7B0EFA566
