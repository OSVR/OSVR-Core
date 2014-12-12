/** @file
    @brief Header

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

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
        /// the
        /// passed JSON configuration
        ///
        /// It first looks for an object under the `server` key - if not
        /// present, it
        /// will assume defaults for below. This object is the context for the
        /// below
        /// elements.
        ///
        /// If `interface` is defined, it will open a server on that interface
        /// (with
        /// an empty string meaning all interfaces)
        /// Otherwise, `local` is assumed to be true. If it is false, it will
        /// open
        /// on all interfaces.
        /// `port` defaults to the assigned VRPN port (3883)
        ///
        /// @throws std::out_of_range if an invalid port (<1) is specified.
        OSVR_SERVER_EXPORT ServerPtr constructServer();

      private:
        unique_ptr<ConfigureServerData> m_data;
        ServerPtr m_server;
    };
} // namespace server
} // namespace osvr

#endif // INCLUDED_ConfigureServer_h_GUID_9D9EE94F_8F85_42BE_AFDD_4AA7B0EFA566
